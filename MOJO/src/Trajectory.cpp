#include "Trajectory.h"


Trajectory::Trajectory(std::string loadPath, std::string kmlPath)
{
    this->loadPath_ = loadPath;
    this->kmlPath_ = kmlPath;
}

int Trajectory::readInputFile(bool isDetection)
{
    if (isDetection) {}
    // {
        std::ifstream file(this->loadPath_);
        std::string line;

        if ((file.is_open()))
        {
            while (std::getline(file, line))
            {
                data_.push_back(line + ',');                
            }
            std::cout << "Finished a reading job" << std::endl;
            file.close();
        }
        else
        {
            std::cerr << "Unable to open file " << this->loadPath_ << std::endl;
            std::cerr << "open() failed: " << std::strerror(errno) << std::endl;
        }
    //}

    // else
    // {}

    return 0;
}