#include "PredictionSupplier.h"

PredictionSupplier::PredictionSupplier(std::string path_to_exe, std::string primaryInputFile) {
    this->path_to_exe_ = path_to_exe;
    this->primaryInputFile_ = primaryInputFile;
}