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
#include <arpa/inet.h>

#define DATA "Half a league, half a league . . .\n"
#define MAX_LINE_LENGTH 1024




int isValidIPv4(const char *ip) {
    struct sockaddr_in sa;
    return inet_pton(AF_INET, ip, &(sa.sin_addr)) != 0;
}

int isValidPort(const char *portStr) {
    int port = atoi(portStr);
    return port > 0 && port <= 65535;
}

int isValidPeriodNs(const char *periodStr) {
    long period = strtol(periodStr, NULL, 10);
    return period > 0 && period <= LONG_MAX;
}

int isValidPriority(const char *priorityStr) {
    int priority = atoi(priorityStr);
    return priority >= 1 && priority <= 99; // POSIX real-time priorities range
}

void printUsage() {
    fprintf(stderr, "Usage: ./rt_sendDetection -i [IP] -j [port] -f [path_to_file] -n [period_ns] -p [priority]\n");
}






/**
 * @brief Arguments to pthread_create()
 */
struct passedArguments_struct
{
    FILE *file_;
    int sock_;
    long period_;
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
 

static void periodic_task_init(struct period_info *pinfo, struct passedArguments_struct *data_ptr)
{     
        // If the flight duration is 223 seconds,
        // and the simulation dt_plot is 0.5 meaning there's 223/0.5 = 446 rows,
        // and I want it to take 7 seconds to show the entire trjectory,
        // that means a coordinate has to be drawn every 7/446 = 0.015695067 seconds
        // which is 15695067.264 nsec
        //pinfo->period_ns = 15695067.264; // xxxx 0.5 sec
        pinfo->period_ns = data_ptr->period_;
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
        printf("Writing a message to socket\n");

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
        struct passedArguments_struct * data_ptr = (struct passedArguments_struct*) data;

        periodic_task_init(&pinfo, data_ptr);
        
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

        const char* IP = "192.168.0.3"; // Default IP
        //const char* PORT = "36961"; // Default port    
        int PORT = 36961; // Default port    
        const char* filePath = "V180.asc"; // Default file path
        long period_ns = 15695067.264; // Default period in nanoseconds
        int rt_priority = 20; // Default POSIX real-time priority


        // Check for the maximum number of arguments (5 flags + 5 values)
        if (argc > 11) {
                fprintf(stderr, "Too many arguments provided!\n");
                printUsage();
                return EXIT_FAILURE;
        }

        // Check arguments passed and their validity, and assign values
        for (int i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-i") == 0) {
                if (i + 1 < argc) {
                        if (!isValidIPv4(argv[i + 1])) {
                        fprintf(stderr, "Invalid IP address.\n");
                        printUsage();
                        return EXIT_FAILURE;
                        }
                        IP = argv[++i];
                } else {
                        fprintf(stderr, "No argument provided for -i.\n");
                        printUsage();
                        return EXIT_FAILURE;
                }
                } else if (strcmp(argv[i], "-j") == 0) {
                if (i + 1 < argc) {
                        if (!isValidPort(argv[i + 1])) {
                        fprintf(stderr, "Invalid port number.\n");
                        printUsage();
                        return EXIT_FAILURE;
                        }
                        PORT = atoi(argv[++i]);
                } else {
                        fprintf(stderr, "No argument provided for -j.\n");
                        printUsage();
                        return EXIT_FAILURE;
                }
                } else if (strcmp(argv[i], "-f") == 0) {
                if (i + 1 < argc) {
                        filePath = argv[++i]; // No validation on file path
                } else {
                        fprintf(stderr, "No argument provided for -f.\n");
                        printUsage();
                        return EXIT_FAILURE;
                }
                } else if (strcmp(argv[i], "-n") == 0) {
                if (i + 1 < argc) {
                        if (!isValidPeriodNs(argv[i + 1])) {
                        fprintf(stderr, "Invalid period in nanoseconds.\n");
                        printUsage();
                        return EXIT_FAILURE;
                        }
                        period_ns = strtol(argv[++i], NULL, 10);
                } else {
                        fprintf(stderr, "No argument provided for -n.\n");
                        printUsage();
                        return EXIT_FAILURE;
                }
                } else if (strcmp(argv[i], "-p") == 0) {
                if (i + 1 < argc) {
                        if (!isValidPriority(argv[i + 1])) {
                        fprintf(stderr, "Invalid POSIX real-time priority.\n");
                        printUsage();
                        return EXIT_FAILURE;
                        }
                        rt_priority = atoi(argv[++i]);
                } else {
                        fprintf(stderr, "No argument provided for -p.\n");
                        printUsage();
                        return EXIT_FAILURE;
                }
                } else {
                fprintf(stderr, "Unknown option: %s\n", argv[i]);
                printUsage();
                return EXIT_FAILURE;
                }
        }       


        param.sched_priority = rt_priority;
        FILE *file = fopen(filePath, "r");
        if (file == NULL) {
            perror("Failed to open file");
            return EXIT_FAILURE;
        }

        // Print the results (for testing purposes)
        printf("IP: %s\n", IP);
        printf("Port: %d\n", PORT);
        printf("File path: %s\n", filePath);
        printf("Period (ns): %ld\n", period_ns);
        printf("Real-time priority: %d\n", rt_priority);


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
        //server.sin_port = htons(atoi(PORT));
        server.sin_port = htons(PORT);
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


