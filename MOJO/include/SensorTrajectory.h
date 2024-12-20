/*=============================================================================
A class derived from 'Trajectory', representing a trajectory that's being
tracked live by a generic sensor.
=============================================================================*/

#pragma once
#include "Trajectory.h"
#include "SyncObject.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

extern int Argc;
extern char** Argv;

class SensorTrajectory : public Trajectory {
    
    public:

    SensorTrajectory(std::string loadPath, std::string kmlPath);
    ~SensorTrajectory() = default;

    BITA_params getBITA_Params() { return this->BITA_Params_; }
    int getCurrentDetectionIndex() { return this->currentDetectionIndex_; }

    /**
     * @brief Get the port for the socket to listen on. If specified as a command
     *        line argument, this argument is taken. Otherwise, sets it to a
     *        default number: 36961.
     */
    void setPortNumber();
    int getPortNumber() { return this->portNumber_;};

    void setDetectionMoment(float value) { detectionMoment_ = value;};
    float getDetectionMoment() { return this->detectionMoment_;};

    void setReachedheightFirstDetection(const bool& value) {reachedheightFirstDetection_ = value;}
    bool getReachedheightFirstDetection_() {return reachedheightFirstDetection_;}
    

    /**
     * @brief Set the trajectory's momentary 'BITA_Params_', as received over the network.
     *        The way of parsing the incoming data into 'BITA_Params' is specific to the source of information.
     */
    virtual void setBITA_Params() = 0;

    /**
     * @return The velocity component perpendicular to Earth's surface.
     */
    virtual float get_vVertical() = 0;
    virtual float getCurrentAlt() = 0;

    /**
     * @brief Listen on a socket. Read data as soon as it's received, and store (Lat, Lon, Alt) coordinates in a dedicated .kml file.
     *        Synchronize with other code sections that rely on the inflowing data. Currently,  supports the incoming of 1 single trajectory.
     *        The way of parsing the incoming data into coordinates is specific to the source of information.
     * @param synchObject Encapsulated synchronization object. 
     */
    virtual void plotDataFromRT(SyncObject* syncObject) = 0;

    std::thread threadReceiveDataFromRT(SyncObject* syncObject) {
        utils::kmlInit_href(this->kmlPath_, this->kmlPath_, this->colorInGE_);
        return std::thread([=, this]{plotDataFromRT(syncObject);});
    }
    
    
    protected:
    
    unsigned int currentDetectionIndex_ = 0; // Index of the last received detection message, containing momentary target information.
    
    private:

    int portNumber_ = 36961;
    float detectionMoment_; // [sec]. In the detected target's own timescale, time passed until detection starts. In reality, this remains unknown, can be estimated.
    bool reachedheightFirstDetection_ = false;

};