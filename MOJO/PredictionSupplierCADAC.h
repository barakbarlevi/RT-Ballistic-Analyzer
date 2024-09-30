/*=============================================================================
'PredictionSupplierCADAC' is derived from 'PredictionSupplier' class.
It's a wrapper around the CADAC simulation.
=============================================================================*/

#pragma once
#include <unordered_map>
#include "PredictionSupplier.h"
#include "PredSuppTrajectoryCADAC.h"

extern int Argc;
extern char** Argv;

class PredictionSupplierCADAC : public PredictionSupplier {

    public:  

    PredictionSupplierCADAC(std::string path_to_exe, std::string priamryInputFile, std::string pathCADAC);
    ~PredictionSupplierCADAC() = default;
    
    virtual int updateBITA_ParamsInSupplierInput(BITA_params BITA_params);

    virtual int runSupplierOnce();

    std::shared_ptr<PredSuppTrajectoryCADAC> getTrajectoryCADAC() const {return trajectoryCADAC_;}
    
    protected:

    private:

    std::string pathCADAC_;
    std::unordered_map<std::string, std::string> EmptyMasses_;
    std::unordered_map<std::string, std::string> Aero_decks_;
    std::shared_ptr<PredSuppTrajectoryCADAC> trajectoryCADAC_;  

};