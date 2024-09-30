/*=============================================================================
'DecisionMaker' is a wrapper around one of the core functionalities in this
project, which is to detect irregularities in the detected trajectory when 
compared to expected simulation results. On initialization, it recevies pointers
to the sensor trajectory and a std::vector whose elements are all suppliers
collectors. calculate() does any custom calculations involving these two.
=============================================================================*/

#include "SuppliersCollector.h"
#include "SensorTrajectoryCADAC.h"

class DecisionMaker {
    
    public:
     
    DecisionMaker(std::vector<std::shared_ptr<SuppliersCollector>>* suppliersCollectorsVector, SensorTrajectoryCADAC* sensorTrajectory);
    ~DecisionMaker() = default;

    void calculate(SyncObject * syncObject);

    /**
     * @brief This thread is calling calculate(). It's created in main(). It runs in the background
              of the main flow stream. The 'SensorTrajectory' of the detected target, as well as a std::vector
              of all 'SuppliersCollector's are passed to this singleton by reference. If some specified 
              condition/s are met within calculate(), it will use 'syncObject' to send a notification. 
              Calculations are synchronized with the incoming flow of detection data.
     */
    std::thread threadCalculate(SyncObject * syncObject) {
        return std::thread([=, this] {calculate(syncObject);});
    }

    protected:

    private:

    SensorTrajectory* sensorTrajectory_;
    std::vector<std::shared_ptr<SuppliersCollector>>* collectorsVector_;
    float estimation_;

};