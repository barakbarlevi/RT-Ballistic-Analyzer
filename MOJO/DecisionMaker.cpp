#include "DecisionMaker.h"

DecisionMaker::DecisionMaker(std::vector<std::shared_ptr<SuppliersCollector>>* suppliersCollectorsVector, SensorTrajectoryCADAC* sensorTrajectory) {
        collectorsVector_ = suppliersCollectorsVector;
        sensorTrajectory_ = sensorTrajectory;
        estimation_ = 0;
    }

void DecisionMaker::calculate(SyncObject * syncObject) {

    int consequtiveConditionCount = 0;
    //float altTolerance = 50; // xxxx
    float altTolerance = 25000;
    float consequtiveConditionCountThreshold = 50;
    float currentTime;
    collectorsVector_->at(1)->getSuppliersVector().at(0)->setAltData();
    collectorsVector_->at(1)->getSuppliersVector().at(0)->setTimeData();
    std::vector<float> collector1supplier0alt = collectorsVector_->at(1)->getSuppliersVector().at(0)->getAltData();
    std::vector<float> collector1supplier0time = collectorsVector_->at(1)->getSuppliersVector().at(0)->getTimeData();

    // xxxx
    std::ofstream writeout;
    writeout.open("MOJO/writeout.txt", std::ios::out | std::ios::binary);

    if (!writeout.is_open())
    {
        std::cerr << "Failed to open the file: writeout.txt" << std::endl;
        std::cerr << "open() failed: " << std::strerror(errno) << std::endl;
        exit(1);
    }

    
    while(syncObject->transmissionEnded_ == false) {

        //printf("Entered while(syncObject->transmissionEnded_ == false)\n");

        std::unique_lock<std::mutex> ul(syncObject->syncMsgStoreAndRead_mutex_);

        //std::cout << "Right before wait(). syncMsgStoreAndRead_ready_: " << syncObject->syncMsgStoreAndRead_ready_ << ". syncObject->transmissionEnded_:" << syncObject->transmissionEnded_ << std::endl;
        
        syncObject->syncMsgStoreAndRead_cv_.wait(ul, [&](){ return (syncObject->syncMsgStoreAndRead_ready_); });

        sensorTrajectory_->setBITA_Params();
        writeout << "Right after sensorTrajectory_->setBITA_Params(). sensorTrajectory_->getBITA_Params().BITA_time: " << sensorTrajectory_->getBITA_Params().BITA_time << std::endl; // xxxx

        currentTime = std::stof(sensorTrajectory_->getBITA_Params().BITA_time);
        auto [closestElement, index] = utils::findClosest(collector1supplier0time, currentTime);

        writeout << "{Closest element, index} in collector1supplier0time: {" << closestElement << "," << index << "}" << ". currentTime: " << currentTime << std::endl; // xxxx

        if(std::abs(sensorTrajectory_->getCurrentAlt() - collector1supplier0alt.at(index)) > std::abs(altTolerance))  {
            consequtiveConditionCount++;


        
            writeout << "Term in collector1supplier0alt: " << collector1supplier0alt.at(index) << std::endl;
            writeout << "getCurrentAlt() : " << sensorTrajectory_->getCurrentAlt() << std::endl;
            writeout << "Their abs difference: " << std::abs(collector1supplier0alt.at(index) - sensorTrajectory_->getCurrentAlt()) << std::endl;


            // std::cout << "Term in collector1supplier0alt: " << collector1supplier0alt.at(index) << std::endl;
            // std::cout << "getCurrentAlt() : " << sensorTrajectory_->getCurrentAlt() << std::endl;
            // std::cout << "Their abs difference: " << std::abs(collector1supplier0alt.at(index) - sensorTrajectory_->getCurrentAlt()) << std::endl;
        }
        //if(!utils::eq(collector1supplier0alt.size(), collector1supplier0alt, sensorTrajectory_->getCurrentAlt(), altTolerance)) { consequtiveConditionCount++; }
        else consequtiveConditionCount = 0;    



        syncObject->syncMsgStoreAndRead_ready_ = false;

        //std::cout << "Assigned syncMsgStoreAndRead_ready_ = false  (from DecisitonMaker.cpp)" << std::endl;
        ul.unlock();

        syncObject->syncMsgStoreAndRead_cv_.notify_one();

        ul.lock();


        // This function doesn't currently address the situation where what once was thought of as
        // an irregularity that justified a notification had sice been restored. 
        if (consequtiveConditionCount >= consequtiveConditionCountThreshold) {
            std::cout << "consequtiveConditionCount >= 5" << std::endl;
            printf("Signaling color\n");
            pthread_cond_signal(&syncObject->condition_variable_color_);
            pthread_mutex_unlock(&syncObject->condition_lock_color_);
            printf("At this stage window should change color to red\n");
        }        
    }

    std::cout << "syncObject->transmissionEnded_:" << syncObject->transmissionEnded_ << std::endl;
    std::cout << "DecisionMaker::calculate finished" << std::endl;
}