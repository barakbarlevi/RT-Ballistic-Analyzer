/*=============================================================================
A class derived from 'Trajectory', representing a trajectory that's the outcome
of some generic simulation.
=============================================================================*/

#pragma once
#include "Trajectory.h"

class PredictionSupplierTrajectory : public Trajectory {
    
    public:

    PredictionSupplierTrajectory(std::string loadPath, std::string kmlPath);
    ~PredictionSupplierTrajectory() = default;

    /**
     * @brief This function edits the approriate .kml file so it will contain everything needed to display
     *        the current 'PredictionSupplierTrajectory' in Google Earth.
     * @param effective_dtPlot Additional control over plot intervals, on top of the simulation output writing interval.
     * @param currentSupplierNumber The sequential number of a single trajectory inside a 'SuppliersCollector'.
     * @param CollectorSize Size of the current 'SuppliersCollector'
     * @param isCollector Whether or not function is called from within a 'SuppliersCollector' context.
     */
    void appendTrajectoryToKML(int effective_dtPlot, int currentSupplierNumber, int CollectorSize, bool isCollector);
    
    void setAltData(); 
    std::vector<float> getAltData() {return altData_; };

    protected:

    unsigned int currentRowIndex_ = 0; // Index of the last received ballistic forecast message from simulation output, containing momentary target information.

    private:

    std::vector<float> altData_; // Holds all the altitude values. It's size is constant per trajectory but data_.size() is detemined at run time.

};