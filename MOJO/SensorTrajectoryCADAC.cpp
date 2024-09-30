#include "SensorTrajectoryCADAC.h"

SensorTrajectoryCADAC::SensorTrajectoryCADAC(std::string loadPath, std::string kmlPath) : SensorTrajectory(loadPath, kmlPath) {
    this->FirstLineOfNumericData_ = 0;
    this->currentDetectionIndex_ = this->FirstLineOfNumericData_;      
}


void SensorTrajectoryCADAC::setBITA_Params()
{
    this->BITA_Params_.BITA_time = utils::SubStringStartTillReaching(data_[currentDetectionIndex_], ',', 1, 0, currentDetectionIndex_, "SensorTrajectoryCADAC::setBITA_Params 1",true);
    // BITA_mass: Cannot be detected by the sensor. It's added in he supplier's 'updateBITA_ParamsInSupplierInput' routine using unordered maps, according to the supplier's model for empty mass after complete burnout.
    this->BITA_Params_.BITA_mass = "0";
    this->BITA_Params_.BITA_lat = utils::SubStringStartTillReaching(data_[currentDetectionIndex_], ',', 5, 1, currentDetectionIndex_, "SensorTrajectoryCADAC::setBITA_Params 2",true);
    this->BITA_Params_.BITA_lon = utils::SubStringStartTillReaching(data_[currentDetectionIndex_], ',', 4, 1, currentDetectionIndex_, "SensorTrajectoryCADAC::setBITA_Params 3",true);
    this->BITA_Params_.BITA_height = utils::SubStringStartTillReaching(data_[currentDetectionIndex_], ',', 6, 1, currentDetectionIndex_, "SensorTrajectoryCADAC::setBITA_Params 4",true);
    this->BITA_Params_.BITA_speed = utils::SubStringStartTillReaching(data_[currentDetectionIndex_], ',', 7, 1, currentDetectionIndex_, "SensorTrajectoryCADAC::setBITA_Params 5",true);
    this->BITA_Params_.BITA_flightPath = utils::SubStringStartTillReaching(data_[currentDetectionIndex_], ',', 3, 1, currentDetectionIndex_, "SensorTrajectoryCADAC::setBITA_Params 6",true);
    this->BITA_Params_.BITA_heading = utils::SubStringStartTillReaching(data_[currentDetectionIndex_], ',', 2, 1, currentDetectionIndex_, "SensorTrajectoryCADAC::setBITA_Params 7",true);
}


void SensorTrajectoryCADAC::setSingleCoordsLine()
{
    std::string lon = utils::SubStringStartTillReaching(this->data_[this->currentDetectionIndex_], ',', 4, 1, currentDetectionIndex_, "SensorTrajectoryCADAC::setSingleCoordsLine 1",true);
    std::string lat = utils::SubStringStartTillReaching(data_[currentDetectionIndex_], ',', 5, 1, currentDetectionIndex_, "SensorTrajectoryCADAC::setSingleCoordsLine 2",true);
    std::string alt = utils::SubStringStartTillReaching(data_[currentDetectionIndex_], ',', 6, 1, currentDetectionIndex_, "SensorTrajectoryCADAC::setSingleCoordsLine 3",true);
    this->SingleCoordsLine_ = lon + "," + lat + "," + alt;
}


void SensorTrajectoryCADAC::plotDataFromRT(SyncObject* syncObject)
{
    int sock;
	socklen_t length;
	struct sockaddr_in server;
	int msgsock;
	char buf[1024];
    std::string buf_string;
	int rval;
    int port;

    utils::kmlInsertOneNetworkLink("Secondary_Controller.kml",this->kmlPath_);        
    this->currentDetectionIndex_ = this->FirstLineOfNumericData_;  

    std::cout << "Initializing server" << std::endl;

    //Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0); // Create a socket that runs use the INET address family. the IP version 4. For a stream.
    if (sock < 0) {
	 //perror("error: opening stream socket");
     std::cerr << "error opening stream socket: " << std::strerror(errno) << std::endl;
	 exit(1);
    }
    
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = 0;
    //server.sin_port = htons(36961);

    try {
        port = getPortNumber();
    } catch (const std::invalid_argument& e) {
        std::cerr << "Caught exception in main: " << e.what() << ". Please inset a suitable integer port number." << std::endl;
        exit(1); // Return a non-zero value to indicate an error
    } catch (const std::out_of_range& e) {
        std::cerr << "Caught exception in main: " << e.what() << ". Please inset a suitable integer port number." << std::endl;
        exit(1); // Return a non-zero value to indicate an error
    }
    std::cout << "Got port number from command line: " << port << std::endl;
    server.sin_port = htons(port);


    // The server has to bind a name on to its socket so that we can know what its address and port are, to tell clients.
    if (bind(sock, (sockaddr*)&server, sizeof(server))) {
	 //perror("error: binding stream socket");
     std::cerr << "error binding stream socket: " << std::strerror(errno) << std::endl;
	 exit(1);
    }

    length = sizeof(server);

    // getsockname - If you have a socket, and you want to know the name it's bound onto it, like the address and the port, we can get it with the function. It will change the arguments, passed as pointers.
    if (getsockname(sock, (sockaddr*)&server, &length)) {
	 //perror("error: getting socket name");
     std::cerr << "error getting socket name: " << std::strerror(errno) << std::endl;
	 exit(1);
    }

    printf("Socket has port #%d\n", ntohs(server.sin_port)); // Convertion from big endian to little endian.

    // Start accepting connections.
    listen(sock, 5);

    // Signal when first message arrives
    do {
        printf("Still waiting. Haven't received first message yet\n");
	    msgsock = accept(sock, 0, 0); // Will block, waiting for a client to do a connect to the address with port from above.
	    if (msgsock == -1)
	        //perror("error: accept");
            std::cerr << "error in accept(): " << std::strerror(errno) << std::endl;
	    else do {
	        bzero(buf, sizeof(buf)); // From John Winan's commentary: aweful. they've (4.4BSD IPC Tutorial) zeroed out this entire buffer every time
            // they read any data in. but! we may or may not get all the data in one single call to read(). just like the other end may or may not
            // write the entire message in one call. it might send it over in chuncks. read() also may not read the whole message on the first call,
            // it might take multiple calls in order to recieve all the bytes in the message. it definitely will, if the message contained more than
            // 1024 bytes. if we didn't get it all on the first try, it will return a nonzero value representing how many bytes it did get, print it
            // and loop. why zeroing the buffer? because if it turns out the buffer has garbage in it, and you read stuff into the buffer that doesn't
            // have a NULL at the end of the buffer when the read call is done, when you print it out it might print garbage forever in the memory of
            // the machine that might follow the end of the buffer. printf(%s, buf) may be an unbounded amount of data. it's due to the fact that read()
            // and write() are not mandated to deliver or transfer as many bytes as you ask in one call. can always give u less. so even though the
            // client sent a bunch of chars and a null char at the end, if read() got all of it in one call, the print statements could be counted on
            // to print the entire content of the buffer and hitting that null. would happen in C++ too. it's a feature of read(). by setting the entire
            // buffer to zero first, you're guaranteed that you'll have a null character at the end of whatever u did recieve. he says it's awful
            // because if rval tells u how many bytes were recieved in this call to read (one line below), if you recieve 1023 instead of 1024, there
            // will always be room for a null at the end of whatever you recieved and down at the printf(%s,buf) statement. could have
            // said buf[rval] = 0. putting 1 null byte here is more efficient than putting a thousand in there every time u go around this loop. 
            if ((rval = read(msgsock, buf, 1024)) < 0) // In this case the fd is a socket, that was returned by accept(). read from my endpoint of this streaming socket, put the data_ inside buf. will read up to 1024 bytes.
                //perror("error: reading stream message");
                std::cerr << "error reading stream message: " << std::strerror(errno) << std::endl;
            if (rval == 0) // got EOF
                printf("Ending connection\n");
            else
            {
                std::cout << "Received first proper message" << std::endl;
                printf("%s\n", buf);

                buf[strlen(buf) - 1] = 0;
                buf_string = buf;
                data_.push_back(buf);
                this->setSingleCoordsLine();

                syncObject->FirstMsgArrived();
            }   
        } while (syncObject->firstMsgArrived_ == false);


        // After first message has already been recieved.
        do {
            bzero(buf, sizeof(buf));
            if ((rval = read(msgsock, buf, 1024)) < 0)
                //perror("error: reading stream message");
                std::cerr << "error reading stream message: " << std::strerror(errno) << std::endl;
            if (rval == 0) // got EOF
                printf("Ending connection\n");
            else
                if(buf[0] == '\0') 
                {
                    std::unique_lock<std::mutex> ul(syncObject->syncMsgStoreAndRead_mutex_);

                    this->currentDetectionIndex_--;

                    syncObject->transmissionEnded_ = true;

                    rval=0;
                    
                    // system() Needed for visualization
                    std::string command = "touch " + this->kmlPath_;
                    std::system(command.c_str());

                    syncObject->syncMsgStoreAndRead_ready_ = true;

                    std::cout << "Assigned syncMsgStoreAndRead_ready_ = true" << std::endl;
                    ul.unlock();

                    syncObject->syncMsgStoreAndRead_cv_.notify_one();
                    
                    ul.lock();

                    if (!getReachedheightFirstDetection_()){
                        syncObject->syncMsgStoreAndRead_cv_.wait(ul, [&](){ return syncObject->syncMsgStoreAndRead_ready_ == false; });
                    }
                }
                else
                {              
                    std::cout << "Getting RT data_ and inserting it to KML: " << this->kmlPath_ << std::endl;
                    printf("rval is:%d\n", rval);
                    printf("Message received: %s", buf);
                    
                    std::unique_lock<std::mutex> ul(syncObject->syncMsgStoreAndRead_mutex_);

                    buf[strlen(buf) - 1] = 0;
                    buf_string = buf;
                    data_.push_back(buf);
                    
                    // This is the reason why this function needs to be implemented from the bottom-most part of the hierarchy
                    this->setSingleCoordsLine();
                    utils::kmlAppendOneCoord(this->kmlPath_, this->SingleCoordsLine_, "0");
                    this->currentDetectionIndex_++;

                    syncObject->syncMsgStoreAndRead_ready_ = true;

                    std::cout << "Assigned syncMsgStoreAndRead_ready_ = true" << std::endl;
                    ul.unlock();

                    syncObject->syncMsgStoreAndRead_cv_.notify_one();
                    
                    ul.lock();

                    if (!getReachedheightFirstDetection_()){
                        syncObject->syncMsgStoreAndRead_cv_.wait(ul, [&](){ return syncObject->syncMsgStoreAndRead_ready_ == false; });
                    }
                }
        } while (rval != 0); // If there was an error and rval was -1, it would still print and won't even break from the loop.. the code simply demostrates socket functionality. taken from BSD's document.

        syncObject->syncMsgStoreAndRead_ready_ = true;

        std::cout << "Assigned syncMsgStoreAndRead_ready_ = true from right before closing socket" << std::endl;
        close(msgsock);
        printf("Closed msgsock\n");
        this->currentDetectionIndex_--;
        //printf("Decremented currentDetectionIndex_ by 1\n");

        
    } while (syncObject->transmissionEnded_ == false);

}