/*=============================================================================
A namespace for some utility functions, mainly involving flat files editing.
=============================================================================*/

#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <cstring>

extern int Argc;
extern char** Argv;

namespace utils {
    
    /**
     * @brief extract a substring from a given string, starting from a
     * @param str The initial string from which we want to extract some portion.
     * @param c The character for which we want to count appearances in the string.
     * @param NumOfEncounterToExclude Number of encounters with 'char c' until fixing the position in the string.
     * @param functionality 0 for trimming the string starting at its first char. 1 for trimming the string starting at the previous 'c' appearance in the string.
     * @param currentDetectionIndex Index of the last received detection message, containing momentary target information.
     * @param caller Caller function, mainly for debug purposes.
     * @param callerIsSensor Boolean, mainly for debug purposes. Either called from sensor or prediction supplier context.
     * @return The resulting std::string. 
     * 
     * The purpose of this function is to retrieve desired values from a string containing momentary target data.
     * For example:
     * 255,-178.767,-33.0216,-120.555,31.2456,64372.8,912.42,-764.855,-16.4627,497.227,380908
     * extract the number after the 5th ',' so it could be later stored in the variable for holding Latitude.                                       
     */
    std::string SubStringStartTillReaching(const std::string &str, char c, int NumOfEncounterToExclude, int functionality, int currentIndex,  std::string caller, bool callerIsSensor);

    
    typedef struct {
        double h;   // Angle in degrees
        double s;   // Fraction between 0 and 1
        double v;   // Fraction between 0 and 1
    } hsv;

    typedef struct {
        double r;   // Fraction between 0 and 1
        double g;   // Fraction between 0 and 1
        double b;   // Fraction between 0 and 1
    } rgb;

    hsv rgb2hsv(rgb in);
    rgb hsv2rgb(hsv in);



    /**
     * @brief Edit 'Primary_Controller.kml' to its initial form.
     *        This file, and only itm has to be manually opened
     *        in Google Earth.
     * @return 0 if the operation was successful, non-zero if an error occurred.
     */
    int kmlInitPrimaryController();

    /**
     * @brief Edit 'Secondary_Controller.kml' to its initial form.
     *        During runtime, hyperlinks to different '.kml' files
     *        represnting ballistic trajectories will be added to
     *        'Secondary_Controller.kml'.
     * @return 0 if the operation was successful, non-zero if an error occurred.
     */
    int kmlInitSecondaryController();


    /**
     * @brief Edit a general '.kml' file that is intended to be filled
     *        with (Lat, Lon, H) data for visualization in Google Earth.
     * @param color This signature is used for the detected target, which
     *              has its own single color.
     * @return 0 if the operation was successful, non-zero if an error occurred.
     */
    int kmlInit_href(std::string kml_path, std::string name, std::string color);

    /**
     * @brief Edit a general '.kml' file that is intended to be filled
     *        with (Lat, Lon, H) data for visualization in Google Earth.
     * @param CollectorSize The size of the collector is used for determining
     *                      the collor of each trajectory inside it.
     * @return 0 if the operation was successful, non-zero if an error occurred.
     */
    int kmlInit_href(std::string kml_path, std::string name, int CollectorSize);

    /**
     * @brief Inser one netowrk link to 'Secondary_Controller.kml'.
     * @return 0 if the operation was successful, non-zero if an error occurred.
     */
    int kmlInsertOneNetworkLink(std::string kml_path, std::string href);
    
    /**
     * @brief Append one (Lat, Lon, H) coordinate inside a '.kml' file
     * @param SingleCoordsLine A string in the form "X,X,X" where X are chars themselves,
     *                         which stand for numbers that can be converted to type float.
     * @param styleID StyleID for visualization purposes.
     * @return 0 if the operation was successful, non-zero if an error occurred.
     */
    int kmlAppendOneCoord(std::string KML, std::string SingleCoordsLine, std::string styleID);


    /**
     * @brief Check equality with tolerance within a range
     * @param m Size of std::vector<Type> x
     * @param x A std::vector
     * @param y The value whose equity is check against x
     * @param altTolerance Tolerance, should account for the summation of many uncertainties.
     * @return false if the deviation is greater than the tolerance, true otherwise.
     */
    template<typename Type>
    bool eq(size_t n, std::vector<Type> x, Type y, Type altTolerance) {
        const Type absTolerance = std::abs(altTolerance);
        for(unsigned int i = 0; i < n; ++i) {
            if (std::abs(x.at(i)-y) > absTolerance) {
                return false;
            }
        }
        return true;
    }

    bool isValidPort(const std::string& arg);
    void displayUsage();

}