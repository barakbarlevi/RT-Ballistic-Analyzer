#include "SuppliersCollector.h"

SuppliersCollector::SuppliersCollector(float timeAfterDetectionCreated) {
        this->currentNumOfSuppliers_ = 0;
        timeAfterDetectionCreated_ = timeAfterDetectionCreated;
    }

void SuppliersCollector::plotCollectorAtOnce(int effective_dtPlot) {

    utils::kmlInit_href(this->collectorKML_, this->collectorKML_, suppliersVector_.size());
    
    for(size_t i = 0; i < suppliersVector_.size(); i++) {
        std::shared_ptr<PredictionSupplierTrajectory> supplierTrajectory = this->suppliersVector_.at(i);
        supplierTrajectory->appendTrajectoryToKML(effective_dtPlot, i, this->suppliersVector_.size(), 1);
    }

    utils::kmlInsertOneNetworkLink("Secondary_Controller.kml",this->collectorKML_);
        
}
