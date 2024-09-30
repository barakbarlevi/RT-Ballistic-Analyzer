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
    int getPortNumber();

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

    int portNumber_ = 0;
    
};