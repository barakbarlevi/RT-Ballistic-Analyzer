/*=============================================================================
'PredictionSupplier' is the base class of its hierarchy. It represents a generic
simulation, or 'Supplier' of ballistic trajectories forecasts, with no specific
context to the output trajectory itself. 
=============================================================================*/

#pragma once
#include "BITA_params.h"


class PredictionSupplier {
    
    public: 
    
    PredictionSupplier(std::string path_to_exe, std::string priamryInputFile);
    ~PredictionSupplier() = default;

    /**
     * @brief A 'PredictionSupplier' is a representing a generic simulation, used to generate
     *        ballisitc trajectories outputs. This function's purpose is to run the simulation once,
     *        in the background of the current code flow.
     */
    virtual int runSupplierOnce() = 0;

    /**
     * @brief Usually a 'PredictionSupplier', namely, a simulation, is run at some initial
     *        conditions stored in one or several input files. The purpose of this function is to
     *        make all edits so that the simulation's input files will contain the momentary
     *        'BITA_params' as initial conditions.
     * @return  0 if the operation was successful, non-zero if an error occurred.
     */
    virtual int updateBITA_ParamsInSupplierInput(BITA_params BITA_params) = 0;

    protected:

    std::string primaryInputFile_; // If the simulation has one input file, this will be a path to it.
    std::string path_to_exe_ = ""; // Path to the simulation executable.

    private:
};