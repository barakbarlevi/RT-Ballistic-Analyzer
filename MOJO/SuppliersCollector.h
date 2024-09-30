/*=============================================================================
'SuppliersCollector' gathers a series of ballistic trajectories forecasts that
 were given at a certain point in time into one entity, uniqely identified by
 this point in time.
=============================================================================*/

#pragma once
#include "PredictionSupplierTrajectory.h"

class SuppliersCollector {
    
    public:
                               
    SuppliersCollector(float timeAfterDetectionCreated);
    ~SuppliersCollector() = default;
    
    void incrementCurrentNumOfSuppliers() {currentNumOfSuppliers_++;}
    void setCollectorKML_(std::string collectorKML) {collectorKML_ = collectorKML;}
    std::string getCollectorKML_() {return collectorKML_;}
    std::vector<std::shared_ptr<PredictionSupplierTrajectory>>& getSuppliersVector()  {return suppliersVector_;}

    /**
     * @brief For visualiztion purposes, add all the trajectories stored in 'suppliersVector_' to Google Earth.
     * @param effective_dtPlot Additional control over plot intervals, on top of the simulation output writing interval. 
     */
    void plotCollectorAtOnce(int effective_dtPlot);


    protected:

    private:

    float timeAfterDetectionCreated_;
    int currentNumOfSuppliers_;
    std::string collectorKML_;
    std::vector<std::shared_ptr<PredictionSupplierTrajectory>> suppliersVector_;

};