/*=============================================================================
'BITA' Stands for "Begin In The Air". These parameters are a capture of a single
moment in time during the flyout of the detected target. There could be multiple
such captures at different times. They are used in the following manner: right
after their capture, 'PredictionSupplier's routine 
'updateBITA_ParamsInSupplierInput' edits a simulation input file to take the
'BITA_params' as initial conditions. This allows for the simulation to generate
a forecast on how a pure ballistic target's motion starting in the current
flight condition will look like until ground hit.
=============================================================================*/


#pragma once
#include <string>

struct BITA_params {
    std::string BITA_time = "0";
    std::string BITA_mass = "0";
    std::string BITA_height = "0";
    std::string BITA_speed = "0";
    std::string BITA_flightPath = "0";
    std::string BITA_heading = "0";
    std::string BITA_lat = "0";
    std::string BITA_lon = "0";
};
