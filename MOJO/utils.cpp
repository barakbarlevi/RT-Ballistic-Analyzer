#include "utils.h"
#include <math.h>

namespace utils
{
    std::string SubStringStartTillReaching(const std::string &str, char c, int NumOfEncounterToExclude, int functionality, int currentIndex,  std::string caller, bool callerIsSensor)
    {
        if(callerIsSensor) std::cout << "currentDetectionIndex: " << currentIndex << " (Caller: " << caller << ")" << std::endl;
        else std::cout << "currentRowIndex: " << currentIndex << " (Caller: " << caller << ")" << std::endl;

        std::vector<size_t> pos;
        for (size_t i = 0; i < str.size(); ++i)
        {
            if (str[i] == c)
            {
                pos.push_back(i);
                if (pos.size() == (size_t)NumOfEncounterToExclude)
                    break;
            }
        }

        if (pos.size() < (size_t)NumOfEncounterToExclude)
        {
            std::cerr << "Size of pos < NumOfEncounterToExclude\n" << "NumOfEncounterToExclude: " << NumOfEncounterToExclude << ", str: " << str << " (Caller: " << caller << "\n";
            std::cout << "SubStringStartTillReaching failed: " << std::strerror(errno) << std::endl;
            return str;
        }
        else
        {
            if (functionality == 0)
                return str.substr(0, pos[NumOfEncounterToExclude - 1]);
            if (functionality == 1)
                return str.substr(pos[NumOfEncounterToExclude - 2] + 1, pos[NumOfEncounterToExclude - 1] - pos[NumOfEncounterToExclude - 2] - 1);
        }
        return "Didn't find any other string";
    }


    hsv rgb2hsv(rgb in)
    {
        hsv out;
        double min, max, delta;

        min = in.r < in.g ? in.r : in.g;
        min = min < in.b ? min : in.b;

        max = in.r > in.g ? in.r : in.g;
        max = max > in.b ? max : in.b;

        out.v = max;
        delta = max - min;
        if (delta < 0.00001)
        {
            out.s = 0;
            out.h = 0; // undefined, maybe nan?
            return out;
        }
        if (max > 0.0)
        {                          // NOTE: if Max is == 0, this divide would cause a crash
            out.s = (delta / max);
        }
        else
        {
            // if max is 0, then r = g = b = 0
            // s = 0, h is undefined
            out.s = 0.0;
            out.h = NAN; // its now undefined
            return out;
        }
        if (in.r >= max)                   // > is bogus, just keeps compilor happy
            out.h = (in.g - in.b) / delta; // between yellow & magenta
        else if (in.g >= max)
            out.h = 2.0 + (in.b - in.r) / delta; // between cyan & yellow
        else
            out.h = 4.0 + (in.r - in.g) / delta; // between magenta & cyan

        out.h *= 60.0; // degrees

        if (out.h < 0.0)
            out.h += 360.0;

        return out;
    }

    rgb hsv2rgb(hsv in)
    {
        double hh, p, q, t, ff;
        long i;
        rgb out;

        if (in.s <= 0.0)
        { // < is bogus, just shuts up warnings
            out.r = in.v;
            out.g = in.v;
            out.b = in.v;
            return out;
        }
        hh = in.h;
        if (hh >= 360.0)
            hh = 0.0;
        hh /= 60.0;
        i = (long)hh;
        ff = hh - i;
        p = in.v * (1.0 - in.s);
        q = in.v * (1.0 - (in.s * ff));
        t = in.v * (1.0 - (in.s * (1.0 - ff)));

        switch (i)
        {
        case 0:
            out.r = in.v;
            out.g = t;
            out.b = p;
            break;
        case 1:
            out.r = q;
            out.g = in.v;
            out.b = p;
            break;
        case 2:
            out.r = p;
            out.g = in.v;
            out.b = t;
            break;

        case 3:
            out.r = p;
            out.g = q;
            out.b = in.v;
            break;
        case 4:
            out.r = t;
            out.g = p;
            out.b = in.v;
            break;
        case 5:
        default:
            out.r = in.v;
            out.g = p;
            out.b = q;
            break;
        }
        return out;
    }



    int kmlInitPrimaryController()
    {
        std::ofstream kml_file;
        kml_file.open("Primary_Controller.kml", std::ios::out | std::ios::binary);

        if (!kml_file.is_open())
        {
            std::cerr << "Failed to open the file: Primary_Controller.kml" << std::endl;
            std::cerr << "open() failed: " << std::strerror(errno) << std::endl;
            return 1;
        }

        kml_file << "<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n";
        kml_file << "\t<Document>\n\n\n";
        kml_file << "\t\t<NetworkLinkControl>\n";
        //kml_file << "\t\t\t<minRefreshPeriod>0.5</minRefreshPeriod>         <!-- float -->\n";
        kml_file << "\t\t\t<minRefreshPeriod>0.01</minRefreshPeriod>         <!-- float -->\n";
        kml_file << "\t\t\t<maxSessionLength>-1</maxSessionLength>          <!-- float -->\n";
        kml_file << "\t\t\t<linkName>Primary_Controller</linkName>          <!-- string -->\n";
        kml_file << "\t\t</NetworkLinkControl>\n\n";
        kml_file << "\t\t<visibility>1</visibility>\n\n";
        kml_file << "\t\t<NetworkLink>\n";
        kml_file << "\t\t\t<name>Main_Network_Link</name>\n";
        kml_file << "\t\t\t<refreshVisibility>1</refreshVisibility>\n";
        kml_file << "\t\t\t<flyToView>0</flyToView>\n";
        kml_file << "\t\t\t<Link>\n";
        kml_file << "\t\t\t\t<href>Secondary_Controller.kml</href>\n";

        kml_file << "\t\t\t\t<refreshMode>onInterval</refreshMode>       <!-- refreshModeEnum: onChange, onInterval, or onExpire -->\n";
        //kml_file << "\t\t\t\t<refreshInterval>0</refreshInterval>        <!-- float -->\n";
        kml_file << "\t\t\t\t<refreshInterval>0.01</refreshInterval>        <!-- float -->\n";
        // kml_file << "\t\t\t\t<refreshMode>onChange</refreshMode>       <!-- refreshModeEnum: onChange, onInterval, or onExpire -->\n";

        kml_file << "\t\t\t\t<viewRefreshMode>never</viewRefreshMode>    <!-- viewRefreshModeEnum: never, onStop, onRequest, onRegion -->\n";
        kml_file << "\t\t\t</Link>\n";
        kml_file << "\t\t</NetworkLink>\n\n\n";

        kml_file << "\t</Document>\n";
        kml_file << "</kml>\n";

        kml_file.close();

        return 0;
    }

    int kmlInitSecondaryController()
    {
        std::ofstream kml_file;
        kml_file.open("Secondary_Controller.kml", std::ios::out | std::ios::binary);

        if (!kml_file.is_open())
        {
            std::cerr << "Failed to open the file: Secondary_Controller.kml" << std::endl;
            std::cerr << "open() failed: " << std::strerror(errno) << std::endl;
            return -1;
        }

        kml_file << "<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n";
        kml_file << "\t<Document>\n\n\n";
        kml_file << "\t\t<NetworkLinkControl>\n";
        //kml_file << "\t\t\t<minRefreshPeriod>0.5</minRefreshPeriod>         <!-- float -->\n";
        kml_file << "\t\t\t<minRefreshPeriod>0.1</minRefreshPeriod>         <!-- float -->\n";
        kml_file << "\t\t\t<maxSessionLength>-1</maxSessionLength>          <!-- float -->\n";
        kml_file << "\t\t\t<linkName>Secondary_Controller</linkName>        <!-- string -->\n";
        kml_file << "\t\t</NetworkLinkControl>\n";
        kml_file << "\t\t<visibility>1</visibility>\n\n\n";

        kml_file << "\t</Document>\n";
        kml_file << "</kml>\n";

        kml_file.close();

        return 0;
    }


    int kmlInit_href(std::string kml_path, std::string name, int CollectorSize)
    {
        std::ofstream kml_file;
        kml_file.open(kml_path);
        if (!kml_file.is_open())
        {
            std::cerr << "Error openning file: " << kml_path << std::endl;
            std::cerr << "open() failed: " << std::strerror(errno) << std::endl;
            return -1;
        }

        std::string line;
        std::string lineToInsert;

        kml_file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\n";
        kml_file << "<kml xmlns=\"http://www.opengis.net/kml/2.2\" xmlns:gx=\"http://www.opengis.net/kml/2.2\" xmlns:kml=\"http://www.opengis.net/kml/2.2\" xmlns:atom=\"http://www.opengis.net/kml/2.2\">\n\n";
        kml_file << "\t<Document>\n";
        kml_file << "\t\t <name>" + name + "</name>\n\n\n";

        utils::rgb RGB;
        utils::hsv HSV;
        std::stringstream ss;

        for (int i = 1; i < CollectorSize + 1; i++)
        {
            ss.str(std::string());
            HSV.h = ((((double)i) / (double)CollectorSize)) * 360.0;
            HSV.s = 0.9;
            HSV.v = 1;
            RGB = utils::hsv2rgb(HSV);

            int decimalR = static_cast<int>(RGB.r * 255);
            int decimalG = static_cast<int>(RGB.g * 255);
            int decimalB = static_cast<int>(RGB.b * 255);
            int decimalA = (1 * 255);

            ss << std::hex << std::setfill('0') << std::setw(2) << decimalA << std::hex << std::setfill('0') << std::setw(2) << decimalG << std::hex << std::setfill('0') << std::setw(2) << decimalB << std::hex << std::setfill('0') << std::setw(2) << decimalR << std::endl;
            std::string color = ss.str();
            
            kml_file << "\t\t <Style id=\"sn_shaded_dot" + std::to_string(i) + "\">\n";
            kml_file << "\t\t\t<IconStyle>\n";
            kml_file << "\t\t\t\t<color>" + color.substr(0, color.size() - 1) + "</color>\n";
            kml_file << "\t\t\t\t<scale>0.4</scale>\n";
            kml_file << "\t\t\t\t<Icon>\n";
            kml_file << "\t\t\t\t\t<href>http://maps.google.com/mapfiles/kml/shapes/shaded_dot.png</href>\n";
            kml_file << "\t\t\t\t</Icon>\n";
            kml_file << "\t\t\t</IconStyle>\n";
            kml_file << "\t\t\t<ListStyle></ListStyle>\n";
            kml_file << "\t\t</Style>\n";
        }

        kml_file << "\t\t<Folder>\n";
        kml_file << "\t\t\t<name>LLAs</name>\n";
        kml_file << "\t\t</Folder>\n";

        kml_file << "\t</Document>\n";
        kml_file << "</kml>\n";

        kml_file.close();
        std::cout << "Closed the file :" << kml_path << "\n";

        return 0;
    }


    int kmlInit_href(std::string kml_path, std::string name, std::string color) {

        std::ofstream kml_file;
        kml_file.open(kml_path);
        if (!kml_file.is_open())
        {
            std::cerr << "Error openning file: " << kml_path << std::endl;
            std::cerr << "open() failed: " << std::strerror(errno) << std::endl;
            return -1;
        }

        std::string line;
        std::string lineToInsert;

        kml_file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\n";
        kml_file << "<kml xmlns=\"http://www.opengis.net/kml/2.2\" xmlns:gx=\"http://www.opengis.net/kml/2.2\" xmlns:kml=\"http://www.opengis.net/kml/2.2\" xmlns:atom=\"http://www.opengis.net/kml/2.2\">\n\n";
        kml_file << "\t<Document>\n";
        kml_file << "\t\t <name>" + name + "</name>\n\n\n";

        kml_file << "\t\t <Style id=\"sn_shaded_dot0\">\n";
        kml_file << "\t\t\t<IconStyle>\n";
        kml_file << "\t\t\t\t<color>" + color + "</color>\n";   
        kml_file << "\t\t\t\t<scale>0.4</scale>\n";
        kml_file << "\t\t\t\t<Icon>\n";
        kml_file << "\t\t\t\t\t<href>http://maps.google.com/mapfiles/kml/shapes/shaded_dot.png</href>\n";
        kml_file << "\t\t\t\t</Icon>\n";
        kml_file << "\t\t\t</IconStyle>\n";
        kml_file << "\t\t\t<ListStyle></ListStyle>\n";
        kml_file << "\t\t</Style>\n";

        kml_file << "\t\t<Folder>\n";
        kml_file << "\t\t\t<name>LLAs</name>\n";
        kml_file << "\t\t</Folder>\n";

        kml_file << "\t</Document>\n";
        kml_file << "</kml>\n";

        kml_file.close();
        std::cout << "Closed the file :" << kml_path << "\n";

        return 0;
    }
    

    int kmlInsertOneNetworkLink(std::string kml_path, std::string href)
    {
        std::fstream kml_file;
        kml_file.open(kml_path, std::ios::in | std::ios::out | std::ios::binary);

        if (!kml_file.is_open())
        {
            std::cerr << "Failed to open the file: " << kml_path << std::endl;
            std::cerr << "open() failed: " << std::strerror(errno) << std::endl;
            return 1;
        }

        kml_file.seekp(-20, kml_file.end);

        kml_file << "\t\t<NetworkLink>\n";
        kml_file << "\t\t\t<name>" + href + "</name>\n";
        kml_file << "\t\t\t<refreshVisibility>1</refreshVisibility>\n";
        kml_file << "\t\t\t<flyToView>0</flyToView>\n";
        kml_file << "\t\t\t<Link>\n";
        kml_file << "\t\t\t\t<href>" + href + "</href>\n";

        kml_file << "\t\t\t\t<refreshMode>onInterval</refreshMode>       <!-- refreshModeEnum: onChange, onInterval, or onExpire -->\n";
        //kml_file << "\t\t\t\t<refreshInterval>0</refreshInterval>        <!-- float -->\n";
        kml_file << "\t\t\t\t<refreshInterval>0.01</refreshInterval>        <!-- float -->\n";
        // kml_file << "\t\t\t\t<refreshMode>onChange</refreshMode>       <!-- refreshModeEnum: onChange, onInterval, or onExpire -->\n";

        kml_file << "\t\t\t\t<viewRefreshMode>never</viewRefreshMode>    <!-- viewRefreshModeEnum: never, onStop, onRequest, onRegion -->\n";
        kml_file << "\t\t\t</Link>\n";
        kml_file << "\t\t</NetworkLink>\n\n\n";

        kml_file << "\t</Document>\n";
        kml_file << "</kml>\n";

        kml_file.close();

        return 0;
    }


    int kmlAppendOneCoord(std::string kml_path, std::string SingleCoordsLine, std::string styleID)
    {
        std::fstream kml_file;
        kml_file.open(kml_path, std::ios::in | std::ios::out | std::ios::binary);

        if (!kml_file.is_open())
        {
            std::cerr << "Failed to open the file: " << kml_path << std::endl;
            std::cerr << "open() failed: " << std::strerror(errno) << std::endl;
            return 1;
        }

        kml_file.seekp(-32, kml_file.end);

        kml_file << "\t\t\t<Placemark>\n";
        kml_file << "\t\t\t<name></name>\n";
        kml_file << "\t\t\t\t<styleUrl>sn_shaded_dot" + styleID + "</styleUrl>\n";
        // kml_file << "\t\t\t\t<description>Time: 15:00</description>\n";
        kml_file << "\t\t\t\t<Point>\n";
        kml_file << "\t\t\t\t\t<altitudeMode>absolute</altitudeMode>\n";
        kml_file << "\t\t\t\t\t<coordinates>" + SingleCoordsLine + "</coordinates>\n";
        kml_file << "\t\t\t\t</Point>\n";
        kml_file << "\t\t\t</Placemark>\n";
        kml_file << "\t\t</Folder>\n";

        kml_file << "\t</Document>\n";
        kml_file << "</kml>\n";

        kml_file.close();

        return 0;
        ;
    }

    bool isValidPort(const std::string& arg) {
    // Check if the string is numeric
    if (arg.empty() || !std::all_of(arg.begin(), arg.end(), ::isdigit)) {
        return false;
    }

    // Convert to an integer
    long port = std::strtol(arg.c_str(), nullptr, 10);

    // Check the port range
    return (port > 0 && port <= 65535);
    }


    void displayUsage() {
    std::cerr << "Usage option 1: " << Argv[0] << " will use default values for the path to the 6-DOF simulation directory and for the port number." << std::endl;
    std::cerr << "Default values: [/home/username/CADAC] , [36961]" << std::endl;
    std::cerr << "Usage option 2: " << Argv[0] << " [path] [port] will use the specified values." << std::endl;
    std::cerr << "No other usage options available" << std::endl;
    }

}
