/*=============================================================================
A class derived from 'PredictionSupplierTrajectory', representing a trajectory
 that's the outcome of simulation: CADAC.
=============================================================================*/

#pragma once
#include "PredictionSupplierTrajectory.h"

class PredSuppTrajectoryCADAC : public PredictionSupplierTrajectory {
    
    public :

    PredSuppTrajectoryCADAC(std::string loadPath, std::string kmlPath);
    ~PredSuppTrajectoryCADAC() = default;

    virtual void setSingleCoordsLine();

    protected:

    private:
};