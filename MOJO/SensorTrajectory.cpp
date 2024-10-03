#include "SensorTrajectory.h"

SensorTrajectory::SensorTrajectory(std::string loadPath, std::string kmlPath) : Trajectory(loadPath, kmlPath) {}

void SensorTrajectory::setPortNumber() {

    for (int i = 1; i < Argc; ++i) {
        if (std::string(Argv[i]) == "-j") {
            if (i + 1 < Argc) {
                try {
                    portNumber_ = std::stoi(Argv[i + 1]); // // Convert string to int
                    if (!utils::isValidPort(Argv[i + 1]))
                        std::cerr << "Error: Port number invalid. Port number must be between 1 and 65535." << std::endl;

                    std::cout << "Got port number from command line: " << portNumber_ << std::endl;

                } catch (const std::invalid_argument& e) {
                    std::cerr << "Error: Argument is not a valid integer." << std::endl;
                    throw; // Re-throw the exception to handle it in main
                } catch (const std::out_of_range& e) {
                    std::cerr << "Error: Argument is out of range for an integer." << std::endl;
                    throw; // Re-throw the exception to handle it in main
                }
                
            } else {
                std::cerr << "Error: -j option requires a port number." << std::endl;
                exit(1);
            }
        }
    }
}