#include "SuppliersCollector.h"

SuppliersCollector::SuppliersCollector(float timeAfterDetectionCreated) {
        this->currentNumOfSuppliers_ = 0;
        timeAfterDetectionCreated_ = timeAfterDetectionCreated;
    }

void SuppliersCollector::plotCollectorAtOnce(int effective_dtPlot) {

    //const char* lastSlash = strrchr(this->collectorKML_, '/'); // Find the last occurrence of '/' // xxxx
    size_t lastSlashPos = this->collectorKML_.find_last_of('/');
    std::string href;

    if (lastSlashPos != std::string::npos) {
        // Get the substring starting right after the last '/'
        href = this->collectorKML_.substr(lastSlashPos + 1);
    } else {
        // If there's no '/', the entire string is the result
        printf("Expecting string: MOJO/CollectorX\n'");
        exit(1);
    }

    utils::kmlInit_href(this->collectorKML_, this->collectorKML_, suppliersVector_.size());
    
    for(size_t i = 0; i < suppliersVector_.size(); i++) {
        std::shared_ptr<PredictionSupplierTrajectory> supplierTrajectory = this->suppliersVector_.at(i);
        supplierTrajectory->appendTrajectoryToKML(effective_dtPlot, i, this->suppliersVector_.size(), 1);
    }

    utils::kmlInsertOneNetworkLink("MOJO/Secondary_Controller.kml",href);
        
}
