#include "SensorTrajectory.h"

SensorTrajectory::SensorTrajectory(std::string loadPath, std::string kmlPath) : Trajectory(loadPath, kmlPath) {}

void SensorTrajectory::setPortNumber() {

    // if (Argc == 3) {
    //     try {
    //         portNumber_ = std::stoi(Argv[2]); // Convert string to int
    //     } catch (const std::invalid_argument& e) {
    //         std::cerr << "Error: Argument is not a valid integer." << std::endl;
    //         throw; // Re-throw the exception to handle it in main
    //     } catch (const std::out_of_range& e) {
    //         std::cerr << "Error: Argument is out of range for an integer." << std::endl;
    //         throw; // Re-throw the exception to handle it in main
    //     }

    // }
    // else if (Argc == 1) portNumber_ = 36961; // Default
    // else {
    //     std::cerr << "Error: argc == 2 | argc > 3, improper usage, exiting" << std::endl;
    //     exit(1);
    // } 
    // return portNumber_;


    //int userPort
    for (int i = 1; i < Argc; ++i) {
        if (std::string(Argv[i]) == "-j") {
            if (i + 1 < Argc) {
                try {
                    portNumber_ = std::stoi(Argv[i + 1]); // // Convert string to int
                    //if (portNumber_ > 0 && portNumber_ <= 65535) {
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
    //return 36961;  // Return default port if not specified
}