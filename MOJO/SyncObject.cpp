#include "SyncObject.h"

SyncObject::SyncObject() {
        pthread_mutex_init(&this->condition_lock_color_, NULL);
        pthread_cond_init(&this->condition_variable_color_, NULL);
        pthread_mutex_init(&this->condition_lock_finished_, NULL);
        pthread_cond_init(&this->condition_variable_finished_, NULL);
        firstMsgArrived_ = false;
    }

void SyncObject::WaitForFirstMsg() {
        std::unique_lock<std::mutex> lk(mtxFirstMsg_);
        cvFirstMsg_.wait(lk, [this]{ 
        return firstMsgArrived_ == true; 
        });
    }

void SyncObject::FirstMsgArrived() {
    {
        std::lock_guard<std::mutex> lk(mtxFirstMsg_);
        firstMsgArrived_ = true;
    }
    cvFirstMsg_.notify_one();
}