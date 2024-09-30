/*=============================================================================
MOJO - a ballistic trajectory analyzer
 
Purpose: Analyze live detection data of a single ballistic target, comparing it
to expected behavior from 6-DOF simulation outputs.
 
Description: The program receives detections live over the network. Detection 
data may contain any information that a generic sensor could be able to 
measure, such as estimated position, velocity, etc. Starting at a pre-
determined point along the trajectory, the program will start executing a
series of 6-DOF simulation runs for an array of potential airframe models,
with the initial conditions being passed to it at the moment, from the tracked
data. The 6-DOF is a separate project. The simulation results for all
airframe models will be stored. The program will continue to receive the live
ballistic target data until it hits the ground, and will be able to generate
simulation outputs during the entire flight time. As soon as the first set of
simulation output is ready, the program will run a background process to compare
target detections and simulation output against each other. This background
process can implement any logic coded into it, its purpose is to send a
notification when some programmer-defined conditions have been met.

Input: The program isn't provided input parameters. It listens on a socket, and
handles data arriving on it.

Output: The program doesn't return a value other than '0' for success. If 
specified conditions relating the target track data and the simulation output
are met, a notification is sent.
   
Notes: 
1. As target detections are simulation outputs themselves, the point at
which calculations on the live 6-DOF executions start can be the detected
target in its starting conditions. This doesn't represent a real-life case for
ballistic targets, since first detection is expected to occur after the object
has gained height. Parameter 'heightFirstDetection' of type double is set for this
reason.
2. In this program's context, the term 'Prediction' refers to any 
simulation's output, that describes the progression of a modeled airframe
motion through the air for the momentary initial conditions. The term
'Supplier' refers to the simulation itself. In this program, the sole
supplier of predictions is the CADAC++ simulation. It's written in a way
that easily allows anyone to add other simulations as a source of information,
with little adjustments to meet their required metadata structure.
3. An effort was made to work according to coding conventions given in
https://geosoft.no/development/cppstyle.html.
=============================================================================*/

// xxxx
// xxxx delete //std::string pathCADAC = home + "/CADAC/"; // xxxx
// xxxx clean green trajectory?

#include "PredictionSupplierCADAC.h"
#include "DecisionMaker.h"
#include "X11_window.h"


enum availableSuppliers {
    CADAC,
    // Add more prediction suppliers (simulations) here.
};

int Argc;
char** Argv;

int main(int argc, char* argv[])
{
    Argc = argc;
    Argv = argv;

    if (argc == 2 || argc > 3) {
        std::cerr << "Error: This program accepts two or no parameters." << std::endl;
        utils::displayUsage();
        return 1;
    }

    char* homeENV = getenv("HOME");
    std::stringstream ss;
    ss.str(homeENV);
    std::string home = ss.str();

    std::string pathCADAC = home + "/Source_Files/CADAC/Custom/Version7/";
    //std::string pathCADAC = home + "/CADAC/"; // xxxx

    if (argc == 3) {
        if(!utils::isValidPort(argv[2]))
        {
            std::cerr << "Port number not valid" << std::endl;
            exit(1);
        }
        pathCADAC = std::string(argv[1]) + "/";
    }

    int effective_dtPlot = 2;
    
    
    // Create a window background thread. Joined at the end of the program, after ground impact.
    pthread_t windowThread;
    SyncObject *syncObject = new SyncObject();
    pthread_create( &windowThread, NULL, windowWork, (void*) syncObject);
    
    

    // The next four assignments can be thought of storing the data for the simulated trajectories in a table, that has constant columns and rows that can change throught the duration of the scenario.
    // We have column 'predictionSuppliers', where each row element is the name of the simulation 'supplying' ballistic trajectories forecasts.
    // Column 'currentCollectorPriamryInputFiles' lists the paths to simulations input files.
    // Column 'currentCollectorExecutables' lists the paths to simulations executable files.
    // Column 'currentCollectorLoadPaths' lists the paths to simulations output files.
    // For example, if we look at the first 'row' of the table below, it states that we'll be running a simulation of type 'CADAC', with file 'inputOriginal.asc' taken as input, executable './SIX_DOF' and it will write its output to 'cadacOutputVDXVDX.asc'.
    // Declaring these as vectors allows for dynamically changing the table's 'rows', enabling the live addition/elimination of airframe models, based on received target data such as energy levels, typical heights and speed etc.
    std::vector<availableSuppliers> predictionSuppliers = {CADAC, CADAC, CADAC};

    std::vector<std::string> currentCollectorPriamryInputFiles = {pathCADAC + "inputOriginal.asc",
                                                                  pathCADAC + "input_Drag0p7.asc",
                                                                  pathCADAC + "input_Drag1p3.asc"};
    std::vector<std::string> currentCollectorExecutables = {pathCADAC + "SIX_DOF",
                                                            pathCADAC + "SIX_DOF",
                                                            pathCADAC + "SIX_DOF"};
    std::vector<std::string> currentCollectorLoadPaths = {pathCADAC + "cadacOutputVDXVDX.asc",
                                                          pathCADAC + "cadacOutputVDXVDX.asc",
                                                          pathCADAC + "cadacOutputVDXVDX.asc"};


    // Initialize necessary .kml files. Only Primary_Controller.kml has to be dragged into Google Earth.
    utils::kmlInitPrimaryController();
    utils::kmlInitSecondaryController();

    // At this point, the detected trajectory object is declared. It has to be declared as the specfic bottom derived
    // class from the Trajectory <- SensorTrajectory <- SensorTrajectory* hierarchy, because we need to know how to parse
    // information in the output file, and the structure of the file may change. Thread receiveDataFromRealtime runs 
    // until the last detection is recevied (ground impact).
    std::string detectionKML = "DetectionRT.kml";
    SensorTrajectoryCADAC trajectoryFromSensor("RT", detectionKML);
    std::thread receiveDataFromRealtime = trajectoryFromSensor.threadReceiveDataFromRT(syncObject);

    
    // Blocks until the first target detetion (message) is recevied over network.
    syncObject->WaitForFirstMsg();

    // Once it's recevied, we no longer wait on it. Reminder: Primary_Controller.kml has to be opened inside Google Earth.
    std::cout << "Started plotting inspected trajectory\n" << std::endl;

        
    #if 1
        
        // A while loop will continuously iterate until a certain condition on the Begin In The Air (BITA) parameters, which are the momentary
        // sampled state vector elements of the detected target, is met. This condtion could be set by the user, and should only rely on
        // detectable parameters. In the example below, we iterate until a certain height at ascent is reached. State vector (position, velocity, etc)
        // is sampled in synchronization with incoming detections: receive message -> sample -> receive message -> sample ....

        float heightFirstDetection(15000); // [meters]
        
        //while ((std::stof(trajectoryFromSensor.BITA_Params_.BITA_height) < heightFirstDetection) && (trajectoryFromSensor.get_vVertical() <= 0))
        while ((std::stof(trajectoryFromSensor.getBITA_Params().BITA_height) < heightFirstDetection) && (trajectoryFromSensor.get_vVertical() <= 0))
        {
            std::unique_lock<std::mutex> ul(syncObject->syncMsgStoreAndRead_mutex_);

            syncObject->syncMsgStoreAndRead_cv_.wait(ul, [&](){ return syncObject->syncMsgStoreAndRead_ready_; });

            // Do work.
            trajectoryFromSensor.setBITA_Params();
            std::cout << "height: " << trajectoryFromSensor.getBITA_Params().BITA_height << std::endl;

            syncObject->syncMsgStoreAndRead_ready_ = false;

            ul.unlock();

            syncObject->syncMsgStoreAndRead_cv_.notify_one();

            ul.lock();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        std::cout << "Reached " << heightFirstDetection << "[m], at currentDetectionIndex: " << trajectoryFromSensor.getCurrentDetectionIndex() << std::endl;
        float detectionTime = std::stof(trajectoryFromSensor.getBITA_Params().BITA_time);
        trajectoryFromSensor.setReachedheightFirstDetection(true);

    #endif


    // Checkpoint: at this stage the entire detected trajectory should be received and plotted, with no analysis preformed on it.
    //receiveDataFromRealtime.join();


    #if 1

        // Once we've met the condition on the former while(), we can start do any kind of analysis we want. Each iteration of the do-while loop below
        // appends a 'Collector' element to a std::vector containing objects of this type. A 'Collector' is an entity that gathers simulation outputs
        // for all airframe models stored in 'currentCollectorPriamryInputFiles', run with the momentary detected target state as initial conditions.
        // In this particular implementation, two 'Collectors' are calculated: the first just as soon as 'heightFirstDetection' is reached, the second
        // some fixed amount of time afterwards. We then exit the loop and run 'DecisionMaker' object method that has access to the entire simulated data
        // as well as to target detection. This method checks whether conditions relating the two are met. One can choose to move it inside the do-while loop.

        std::vector<std::shared_ptr<SuppliersCollector>> suppliersCollectorsVector;
        DecisionMaker decisionMaker(&suppliersCollectorsVector, &trajectoryFromSensor);

        do
        {
            std::shared_ptr<SuppliersCollector> currentCollector = std::make_shared<SuppliersCollector>(std::stof(trajectoryFromSensor.getBITA_Params().BITA_time) - detectionTime);
            suppliersCollectorsVector.push_back(currentCollector);
            
            currentCollector->setCollectorKML_("Collector" + std::to_string(suppliersCollectorsVector.size() - 1) + ".kml");

            // Iterating over all the airframe model we currently want to check. Those are stored in a std::vector and can be added/removed during the scenario.
            // This loop is representing a single point in time where we sample the detected data, insert it to all relevant simulation input files one by one,
            // run the simulation and store its outputs. The next time this loop is executed is at a subsequent time during the flight, with different detected
            // target conditions.
            for (size_t i = 0; i < currentCollectorPriamryInputFiles.size(); ++i)
            {                                                                     
                switch (predictionSuppliers.at(i))
                {
                    case CADAC:
                    {
                        std::shared_ptr<PredictionSupplierCADAC> predictionSupplierCADAC = std::make_shared<PredictionSupplierCADAC>(currentCollectorExecutables.at(i), currentCollectorPriamryInputFiles.at(i), pathCADAC);

                        // Injecting the state of the detected target into the current input file for a simulations.
                        // At the first time executing this line, BITA params are from 'heightFirstDetection'.
                        predictionSupplierCADAC->updateBITA_ParamsInSupplierInput(trajectoryFromSensor.getBITA_Params());

                        // Run a single simulation run with the adequate initial conditions.
                        predictionSupplierCADAC->runSupplierOnce();

                        // Set the right load path and kml path for the newly instantiated 'PredSuppTrajectoryCADAC'
                        predictionSupplierCADAC->getTrajectoryCADAC()->setLoadPath(currentCollectorLoadPaths.at(i));
                        predictionSupplierCADAC->getTrajectoryCADAC()->set_kmlPath(currentCollector->getCollectorKML_());

                        // Read simulation results
                        predictionSupplierCADAC->getTrajectoryCADAC()->readInputFile(false);
                                                
                        // Push the simulation results to the correct place inside 'suppliersCollectorsVector's back().
                        suppliersCollectorsVector.back()->getSuppliersVector().push_back(predictionSupplierCADAC->getTrajectoryCADAC());
                    }
                }
                // Increment the number of suppliers residing in the current 'SuppliersCollector'.
                suppliersCollectorsVector.back()->incrementCurrentNumOfSuppliers();
            }

            suppliersCollectorsVector.back()->plotCollectorAtOnce(effective_dtPlot);
            std::this_thread::sleep_for(std::chrono::milliseconds(1450));


            std::unique_lock<std::mutex> ul(syncObject->syncMsgStoreAndRead_mutex_);
            syncObject->syncMsgStoreAndRead_cv_.wait(ul, [&](){ return syncObject->syncMsgStoreAndRead_ready_; });
            // Do work. Capture detected target state
            trajectoryFromSensor.setBITA_Params();
            std::cout << "height: " << trajectoryFromSensor.getBITA_Params().BITA_height << std::endl;
            syncObject->syncMsgStoreAndRead_ready_ = false;
            ul.unlock();
            syncObject->syncMsgStoreAndRead_cv_.notify_one();
            ul.lock();


        } while (suppliersCollectorsVector.size() <= 1);
        
        // Start running the DecisionMaker object calculations. One can choose to move this elsewhere in the code, as long as there's enough information to handle.
        std::thread giveEstimation = decisionMaker.threadCalculate(syncObject);
        
        receiveDataFromRealtime.join();
        giveEstimation.join();

        // Needed for visualization
        std::string command = "touch " + detectionKML;
        std::system(command.c_str());
        
        std::this_thread::sleep_for(std::chrono::seconds(1));

        printf("Signaling \"finished\" to close the text box.\n");
        pthread_cond_signal(&syncObject->condition_variable_finished_);
        pthread_mutex_unlock(&syncObject->condition_lock_finished_);
        pthread_join(windowThread, NULL);

    #endif
    
    // Needed for visualization
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::string command2 = "touch " + detectionKML;
    std::system(command2.c_str());

    delete syncObject;

    return 0;
}