/*=============================================================================
Encapsulating synchronization for the entire program.

Case1: Send a signal from std::thread 'receiveDataFromRealtime' to the main
thread when the first message in the sequence has arrived over the socket. This
is done in function 'FirstMsgArrived()'. in main(), function 'WaitForFirstMsg()'
is blocking.

Case2: Producer-consumer model for synchronizing storing the relevant data
from messages recevied via network, and actually using it to:
(a) in main(): set 'BITA_Params' both in the while() loop awaiting to reach
'heightFirstDetection' (any other condition could be set) and in the do-while()
loop iterating over 'suppliersCollectorsVector', and
(b) In 'DecisionMaker::calculate' read the current state of the target.

Case3: Send a signal from std::thread 'giveEstimation' to pthread_t 'windowThread'
that an irregularity has been detected, so the latter can now proceed to change
the text box attributes.

Case4: Assign 'transmissionEnded_' to true when the detections file on the 
client side has reached EOF, so that 'DecisionMaker::calculate()' will exit its
while() iterations, complete its execution, and std::thread 'giveEstimation' could
join.

Case5: Send a signal from the main thread to pthread_t 'windowThread', notifying
it that the program has finished executing all it needed so it can properly 
close the window.
=============================================================================*/

#pragma once
#include <mutex>
#include <condition_variable>

class SyncObject {

    public:

    SyncObject();
    ~SyncObject() = default;

    bool firstMsgArrived_;
    bool transmissionEnded_ = false;

    std::condition_variable cvFirstMsg_;
    std::mutex mtxFirstMsg_;

    bool              condition_boolean_color_ = false;
    pthread_mutex_t  condition_lock_color_;
    pthread_cond_t   condition_variable_color_;

    pthread_mutex_t  condition_lock_finished_;
    pthread_cond_t   condition_variable_finished_;

    std::mutex syncMsgStoreAndRead_mutex_;
    std::condition_variable syncMsgStoreAndRead_cv_;
    bool syncMsgStoreAndRead_ready_ = false;
    
    /**
     * @brief See 'Case1' in the description above.
     */
    void WaitForFirstMsg();
    void FirstMsgArrived();

};