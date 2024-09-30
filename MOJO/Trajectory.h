/*=============================================================================
'Trajectory' is the base class of its hierarchy. It hold attributes which are
generic for any kind of a ballistic trajectory, whether it's received live
by a sensor, or an output file of a simulation.
=============================================================================*/

#pragma once
#include <mutex>
#include <condition_variable>
#include "utils.h"
#include "BITA_params.h"

class Trajectory {
    
    public:

    Trajectory(std::string loadPath, std::string kmlPath);
    ~Trajectory() = default;

    /**
     * @brief Read trajectory input file line by line.
     * @param isDetection is currently unused. Left as a parameter for possible future implementations.
     * @return 0 if the operation was successful, non-zero if an error occurred.
     */
    virtual int readInputFile(bool isDetection);

    /**
     * @brief Set the momentary (Lat, Lon, Height) coordinate of the trajectory.
     */
    virtual void setSingleCoordsLine() = 0;
    
    bool getReachedheightFirstDetection_() {return reachedheightFirstDetection_;}
    BITA_params getBITA_Params() {return BITA_Params_;}
    void setReachedheightFirstDetection(const bool& value) {reachedheightFirstDetection_ = value;}
    void setLoadPath(const std::string& value) {loadPath_ = value;}
    void set_kmlPath(const std::string& value) {kmlPath_ = value;}


    protected:

    std::string loadPath_; // The path from which trajectory data is loaded.
    std::string kmlPath_; // The path to the .kml file that will contain the trajectory data to visualize in Google Earth.
    std::string SingleCoordsLine_; // Holds the momentary (Lat, Lon, Height) coordinate of the trajectory.
    std::string colorInGE_ =  "ff00AAff"; // Default color for Google Earth display
    std::string scale_ =  "0.4"; // Default scale for Google Earth display
    std::vector<std::string> data_;  // Contains any data the relevant for the trajectory: time, position, flight path and heading angles, etc.
    BITA_params BITA_Params_; // Momentary state vector, containing any data that can be measured for the detected target: position, velocity, heading etc.
    int FirstLineOfNumericData_; // Output files for different simulations vary in their structure. The program reads the file line by line. This is the index of the first line containing numerical values to parse.

    private:

    bool reachedheightFirstDetection_ = false;
    
};
