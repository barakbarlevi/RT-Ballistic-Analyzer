#include "PredictionSupplierCADAC.h"

PredictionSupplierCADAC::PredictionSupplierCADAC(std::string path_to_exe, std::string priamryInputFile, std::string pathCADAC) : PredictionSupplier(path_to_exe, priamryInputFile)
{
    this->path_to_exe_ = path_to_exe;           
    this->primaryInputFile_ = priamryInputFile;
    this->pathCADAC_ = pathCADAC;
    
    this->trajectoryCADAC_ = std::make_shared<PredSuppTrajectoryCADAC>("Empty", "Empty");

    EmptyMasses_ = {
        {pathCADAC + "inputOriginal.asc", "17809"},
        {pathCADAC + "input_Drag0p7.asc", "17809"},
        {pathCADAC + "input_Drag1p3.asc", "17809"}
    };

    Aero_decks_ = {
        {pathCADAC + "inputOriginal.asc", "aero_deck_SLV.asc"},
        {pathCADAC + "input_Drag0p7.asc", "aero_deck_SLV_Drag0p7.asc"},
        {pathCADAC + "input_Drag1p3.asc", "aero_deck_SLV_Drag1p3.asc"}
    };

}


int PredictionSupplierCADAC::updateBITA_ParamsInSupplierInput(BITA_params BITA_params)
{
    
    std::string inputPath = this->primaryInputFile_;
    std::ofstream input_file(inputPath, std::ofstream::out | std::ios::binary);

    if (!input_file.is_open())
    {
        std::cerr << "Failed to open the file: " << this->primaryInputFile_ << std::endl;
        std::cerr << "open() failed: " << std::strerror(errno) << std::endl;
    }
    std::string momentaryMass, aeroDeck;

    // This code is restricted to only give ballistic forecasts for non-propelled flight. Looking up empty masses for a lookup table.
    if (this->EmptyMasses_.find(this->primaryInputFile_) != this->EmptyMasses_.end()) { momentaryMass = this->EmptyMasses_[this->primaryInputFile_]; }
    else{
        std::cerr << "Key not found in EmptyMass unordered map.\n" << std::endl;
        return -1;
    }
        
    if (this->Aero_decks_.find(this->primaryInputFile_) != this->Aero_decks_.end()) { aeroDeck = this->Aero_decks_[this->primaryInputFile_]; }
    else
    {
        std::cerr << "Key not found in AeroDecks unordered map.\n" << std::endl;
        return -1;
    }
        
    input_file << "TITLE input_ballistic.asc  Three-stage rocket ascent followed by ballistic\nMONTE 1 1234\nOPTIONS y_plot\nMODULES\nkinematics		def,init,exec\nenvironment		def,init,exec\npropulsion		def,init,exec\naerodynamics	def,init,exec\nforces			def,exec\nnewton			def,init,exec\neuler			def,init,exec\nintercept		def,exec\nEND\nTIMING\nscrn_step 10\nplot_step 0.5\ntraj_step 1\nint_step 0.003\ncom_step 20\nEND\nVEHICLES 1\nHYPER6 SLV\nlonx  " + BITA_params.BITA_lon + "\nlatx  " + BITA_params.BITA_lat + "\nalt  " + BITA_params.BITA_height + "\ndvbe  " + BITA_params.BITA_speed + "\nphibdx  0\nthtbdx  " + BITA_params.BITA_flightPath + "\npsibdx  " + BITA_params.BITA_heading + "\nalpha0x  0\nbeta0x  0\n//environment\nmair  0\nWEATHER_DECK "  + this->pathCADAC_ + "weather_deck_Wallops.asc\nRAYL dvae  5\ntwind  1\nturb_length  100\nturb_sigma  0.5\n//aerodynamics\nmaero  11\nAERO_DECK " + this->pathCADAC_ + aeroDeck + "\nxcg_ref  0.01\nrefa  3.243\nrefd  2.032\nalplimx  20\nalimitx  5\n//propulsion\nmprop  0\nvmass0  " + momentaryMass + "\nfmass0  0.01\nxcg_0  0.01\nxcg_1  0.01\nmoi_roll_0  6.95e3\nmoi_roll_1  6.95e3\nmoi_trans_0  158.83e3\nmoi_trans_1  158.83e3\nspi  0.01\nfuel_flow_rate  0.0\nEND\nENDTIME 900\nSTOP";
    
    /*
    kml_file << "TITLE input_ballistic.asc  Three-stage rocket ascent followed by ballistic\n \
                MONTE 1 1234\n \
                OPTIONS y_plot\n \
                MODULES\n \
                kinematics		def,init,exec\n \
                environment		def,init,exec\n \
                propulsion		def,init,exec\n \
                aerodynamics	def,init,exec\n \
                forces			def,exec\n \
                newton			def,init,exec\n \
                euler			def,init,exec\n \
                intercept		def,exec\n \
                END\n \
                TIMING\n \
                scrn_step 10\n \
                plot_step 0.5\n \
                traj_step 1\n \
                int_step 0.003\n \
                com_step 20\n \
                END\n \
                VEHICLES 1\n \
                HYPER6 SLV\n \
                lonx  " + BITA_params.BITA_lon + "\n \
                latx  " + BITA_params.BITA_lat + "\n \
                alt  " + BITA_params.BITA_height + "\n \
                dvbe  " + BITA_params.BITA_speed + "\n \
                phibdx  0\n \
                thtbdx  " + BITA_params.BITA_flightPath + "\n \
                psibdx  " + BITA_params.BITA_heading + "\n \
                alpha0x  0\n \
                beta0x  0\n \
                //environment\n \
                mair  0\n \
                WEATHER_DECK  weather_deck_Wallops.asc\n \
                RAYL dvae  5\n \
                twind  1\n \
                turb_length  100\n \
                turb_sigma  0.5\n \
                //aerodynamics\n \
                maero  13\n \
                AERO_DECK " + aeroDeck + "\n \
                xcg_ref  8.6435\n \
                refa  3.243\n \
                refd  2.032\n \
                alplimx  20\n \
                alimitx  5\n \
                //propulsion\n \
                mprop  3\n \
                vmass0  " + momentaryMass + "\n \
                fmass0  0\n \
                xcg_0  7.02\n \
                xcg_1  6.98\n \
                moi_roll_0  21.94e3\n \
                moi_roll_1  6.95e3\n \
                moi_trans_0  671.62e3\n \
                moi_trans_1  158.83e3\n \
                spi  279.2\n \
                fuel_flow_rate  514.1\n \
                END\n \
                ENDTIME 900\n \
                STOP";
    */

    input_file.close();
    std::cout << "Wrote to input file: lonx = " + BITA_params.BITA_lon + ", latx = " + BITA_params.BITA_lat + ", alt = " + BITA_params.BITA_height + ", dvbe = " + BITA_params.BITA_speed + ", thtbdx = " + BITA_params.BITA_flightPath + ", psibdx = " + BITA_params.BITA_heading + ", Aerodeck = " + aeroDeck + ", vmass0 = " + momentaryMass << std::endl;
    return 0;
}

int PredictionSupplierCADAC::runSupplierOnce()
{ 
    if (Argc == 3) {
        try {
            std::string COMMAND = this->path_to_exe_ + " " + this->primaryInputFile_ + " " + Argv[1];
            if (std::system(COMMAND.c_str()) != 0) {
            std::cerr << "Command: " << COMMAND << " Failed.\nCheck path to 6-DOF. Exiting" << std::endl;
            exit(1);
    }
        } catch (const std::invalid_argument& e) {
            std::cerr << "Error: Argument is not a valid integer." << std::endl;
            throw; // Re-throw the exception to handle it in main
        } catch (const std::out_of_range& e) {
            std::cerr << "Error: Argument is out of range for an integer." << std::endl;
            throw; // Re-throw the exception to handle it in main
        }

    }
    else if (Argc == 1) {
        std::string COMMAND = this->path_to_exe_ + " " + this->primaryInputFile_;
        if (std::system(COMMAND.c_str()) != 0) {
        std::cerr << "Command: " << COMMAND << " Failed.\nCheck path to 6-DOF. Exiting" << std::endl;
        exit(1);
    }
    } 
    else {
        std::cerr << "Error: argc == 2 | argc > 3, improper usage, exiting" << std::endl;
        exit(1);
    } 

    return 0;
}