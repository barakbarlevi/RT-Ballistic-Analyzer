#include "PredSuppTrajectoryCADAC.h"

PredSuppTrajectoryCADAC::PredSuppTrajectoryCADAC(std::string loadPath, std::string kmlPath) : PredictionSupplierTrajectory(loadPath, kmlPath) { 
    this->FirstLineOfNumericData_ = 0; 
    this->currentRowIndex_ = this->FirstLineOfNumericData_;
    
};

void PredSuppTrajectoryCADAC::setSingleCoordsLine()
{
    std::string lon = utils::SubStringStartTillReaching(this->data_[this->currentRowIndex_], ',', 4, 1, currentRowIndex_, "PredSuppTrajectoryCADAC::setSingleCoordsLine 1",false);
    std::string lat = utils::SubStringStartTillReaching(data_[currentRowIndex_], ',', 5, 1, currentRowIndex_, "PredSuppTrajectoryCADAC::setSingleCoordsLine 2",false);
    std::string alt = utils::SubStringStartTillReaching(data_[currentRowIndex_], ',', 6, 1, currentRowIndex_, "PredSuppTrajectoryCADAC::setSingleCoordsLine 3",false);

    this->SingleCoordsLine_ = lon + "," + lat + "," + alt;
}