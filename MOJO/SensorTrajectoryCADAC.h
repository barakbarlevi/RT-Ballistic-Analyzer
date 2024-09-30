/*=============================================================================
A class derived from 'SensorTrajectory', representing a trajectory that's being
tracked live by the specific sensor: CADAC. This isn't an actual sensor. CADAC
simulation is currently taking the place of an actualy sensor, providing
'track' data - simulation output that represents track detections.
=============================================================================*/

#pragma once
#include "SensorTrajectory.h"


class SensorTrajectoryCADAC : public SensorTrajectory {
    
    public:

    SensorTrajectoryCADAC(std::string loadPath, std::string kmlPath);
    ~SensorTrajectoryCADAC() = default;

    virtual void setSingleCoordsLine();
    virtual void setBITA_Params(); 
    
    float get_vVertical() { return std::stof(utils::SubStringStartTillReaching(this->data_[currentDetectionIndex_], ',', 10, 1, currentDetectionIndex_, "SensorTrajectoryCADAC::get_vVertical",true)); }
    
    float getCurrentAlt() { 
        return std::stof(utils::SubStringStartTillReaching(data_[currentDetectionIndex_], ',', 6, 1, currentDetectionIndex_, "SensorTrajectory::getCurrentAlt",true));
    }

    virtual void plotDataFromRT(SyncObject* syncObject);

    protected:

    private:

};
