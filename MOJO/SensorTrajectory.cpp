#include "SensorTrajectory.h"

SensorTrajectory::SensorTrajectory(std::string loadPath, std::string kmlPath) : Trajectory(loadPath, kmlPath) {}

int SensorTrajectory::getPortNumber() {

    if (Argc == 3) {
        try {
            portNumber_ = std::stoi(Argv[2]); // Convert string to int
        } catch (const std::invalid_argument& e) {
            std::cerr << "Error: Argument is not a valid integer." << std::endl;
            throw; // Re-throw the exception to handle it in main
        } catch (const std::out_of_range& e) {
            std::cerr << "Error: Argument is out of range for an integer." << std::endl;
            throw; // Re-throw the exception to handle it in main
        }

    }
    else if (Argc == 1) portNumber_ = 36961; // Default
    else {
        std::cerr << "Error: argc == 2 | argc > 3, improper usage, exiting" << std::endl;
        exit(1);
    } 
    return portNumber_;
}