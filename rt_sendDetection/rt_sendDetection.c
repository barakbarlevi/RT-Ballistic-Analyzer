/*=============================================================================
POSIX real-time application.
Code structure base:
https://wiki.linuxfoundation.org/realtime/documentation/howto/applications/cyclic
This program starts a real-time cyclic task thread that sends a message
on a socket once every 'period_ns' time portion in nanoseconds. The messages can be 
thought of track data coming from a sensor.The periodicity simulates live target
tracking.
=============================================================================*/

 
#include <limits.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

#define DATA "Half a league, half a league . . .\n"
#define MAX_LINE_LENGTH 1024

/**
 * @brief Arguments to pthread_create()
 */
struct passedArguments_struct
{
    FILE *file_;
    int sock_;
};


struct period_info {
        struct timespec next_period;
        long period_ns;
};
 

static void inc_period(struct period_info *pinfo) 
{
        pinfo->next_period.tv_nsec += pinfo->period_ns;
 
        while (pinfo->next_period.tv_nsec >= 1000000000) {
                // timespec nsec overflow
                pinfo->next_period.tv_sec++;
                pinfo->next_period.tv_nsec -= 1000000000;
        }
}
 

static void periodic_task_init(struct period_info *pinfo)
{     
        // If the flight duration is 223 seconds,
        // and the simulation dt_plot is 0.5 meaning there's 223/0.5 = 446 rows,
        // and I want it to take 7 seconds to show the entire trjectory,
        // that means a coordinate has to be drawn every 7/446 = 0.015695067 seconds
        // which is 15695067.264 nsec
        pinfo->period_ns = 15695067.264; // xxxx 0.5 sec
        //pinfo->period_ns = 31390134.528; // xxxx *2
        //pinfo->period_ns = 47085201.792; // xxxx *3
        //pinfo->period_ns = 62780269.056; // xxxx *4
        //pinfo->period_ns = 78475336.32; // xxxx *5
        
        //pinfo->period_ns = 1000000000; // xxxx 1 sec
        //pinfo->period_ns = 1000000; // xxxx 1 msec
        
        clock_gettime(CLOCK_MONOTONIC, &(pinfo->next_period));
}

 
static void do_rt_task(FILE* file, int sock)
{
        // Do RT stuff here.
        printf("Hi\n");

        char line[MAX_LINE_LENGTH]{};

        fgets(line, sizeof(line), file);
        if (write(sock,line,sizeof(line)) < 0)
                perror("error writing on stream socket");

}


static void wait_rest_of_period(struct period_info *pinfo)
{
        inc_period(pinfo);
 
        // for simplicity, ignoring possibilities of signal wakes
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &pinfo->next_period, NULL);
}


void *simple_cyclic_task(void *data)
{
        // Do RT specific stuff here
        struct period_info pinfo;
 
        periodic_task_init(&pinfo);
        
        struct passedArguments_struct * data_ptr = (struct passedArguments_struct*) data;
        int sock = data_ptr->sock_;
        FILE* file = data_ptr->file_;

        while (1)
        {
                do_rt_task(file, sock);
                wait_rest_of_period(&pinfo);
        }

        close(sock);
        printf("Closed socket\n");

        return NULL;
}
 
int main(int argc, char* argv[])
{
        struct sched_param param;
        pthread_attr_t attr;
        pthread_t thread;
        
        int ret;
        
        int sock;
        struct sockaddr_in server;
        struct hostent *hp;
        struct passedArguments_struct data;

        const char* IP = "192.168.0.3";
        const char* PORT = "36961";
        param.sched_priority = 80;
        FILE *file = fopen("V180.asc", "r");
        if (file == NULL) {
            perror("Failed to open file");
            return EXIT_FAILURE;
        }


        
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            perror("Error openning stream socket");
            exit(1);
        }
        server.sin_family = AF_INET;
        hp = gethostbyname(IP);
        if (hp == 0)
        {
            fprintf(stderr, "%s: unknown host", IP);
            exit(2);
        }
        bcopy(hp->h_addr, &server.sin_addr, hp->h_length);
        server.sin_port = htons(atoi(PORT));
        if (connect(sock, (sockaddr*)&server, sizeof(server)) < 0) {
            perror("Error connecting stream socket");
            exit(1);
        }

        
        data.file_ = file;
        data.sock_ = sock;

        ret = pthread_attr_init(&attr);
        if (ret) {
                printf("init pthread attributes failed\n");
                goto out;
        }

        // Lock memory 
        if(mlockall(MCL_CURRENT|MCL_FUTURE) == -1) {
                printf("mlockall failed: %m\n");
                exit(-2);
        }
 
        // Initialize pthread attributes (default values)
        ret = pthread_attr_init(&attr);
        if (ret) {
                printf("init pthread attributes failed\n");
                goto out;
        }
 
        // Set a specific stack size  
        ret = pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);
        if (ret) {
            printf("pthread setstacksize failed\n");
            goto out;
        }
 
        // Set scheduler policy and priority of pthread 
        ret = pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
        if (ret) {
                printf("pthread setschedpolicy failed\n");
                goto out;
        }

        
        ret = pthread_attr_setschedparam(&attr, &param);
        if (ret) {
                printf("pthread setschedparam failed\n");
                goto out;
        }

        // Use scheduling parameters of attr
        ret = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
        if (ret) {
                printf("pthread setinheritsched failed\n");
                goto out;
        }
 
        // Create a pthread with specified attributes
        ret = pthread_create(&thread, &attr, simple_cyclic_task, &data);
        if (ret) {
                printf("create pthread failed\n");
                goto out;
        }
 
        // Join the thread and wait until it is done 
        ret = pthread_join(thread, NULL);
        if (ret)
                printf("join pthread failed: %m\n");
 
out:
        return ret;
}


