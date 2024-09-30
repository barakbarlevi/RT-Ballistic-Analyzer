#include "DecisionMaker.h"

DecisionMaker::DecisionMaker(std::vector<std::shared_ptr<SuppliersCollector>>* suppliersCollectorsVector, SensorTrajectoryCADAC* sensorTrajectory) {
        collectorsVector_ = suppliersCollectorsVector;
        sensorTrajectory_ = sensorTrajectory;
        estimation_ = 0;
    }

void DecisionMaker::calculate(SyncObject * syncObject) {

    int consequtiveConditionCount = 0;
    float altTolerance = 50;
    float consequtiveConditionCountThreshold = 50;
    collectorsVector_->at(1)->getSuppliersVector().at(0)->setAltData();
    std::vector<float> collector1supplier0alt = collectorsVector_->at(1)->getSuppliersVector().at(0)->getAltData();
    
    while(syncObject->transmissionEnded_ == false) {

        //printf("Entered while(syncObject->transmissionEnded_ == false)\n");

        std::unique_lock<std::mutex> ul(syncObject->syncMsgStoreAndRead_mutex_);

        //std::cout << "Right before wait(). syncMsgStoreAndRead_ready_: " << syncObject->syncMsgStoreAndRead_ready_ << ". syncObject->transmissionEnded_:" << syncObject->transmissionEnded_ << std::endl;
        
        syncObject->syncMsgStoreAndRead_cv_.wait(ul, [&](){ return (syncObject->syncMsgStoreAndRead_ready_); });

        if(!utils::eq(collector1supplier0alt.size(), collector1supplier0alt, sensorTrajectory_->getCurrentAlt(), altTolerance)) { consequtiveConditionCount++; }
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