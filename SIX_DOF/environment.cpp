///////////////////////////////////////////////////////////////////////////////
//FILE: 'environment.cpp'
//Contains 'environment' module of class 'Round6'
//
//030507 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////

#include "class_hierarchy.hpp"

///////////////////////////////////////////////////////////////////////////////
//Definition of environment module-variables 
//Member function of class 'Round6'
//Ref: Zipfel, Section 10.3.2, p. 465
//Module-variable locations are assigned to round6[50-99]
// 
// The switch 'mair' controls the atmosphere, wind and air turbulence options:
//
//     mair=|matmo|mturb|mwind|
//
//		     matmo = 0 US 1976 Standard Atmosphere (public domain shareware)
//				   = 1 US 1976 Standard Atmosphere with extension up to 1000 km (NASA Marshall)
//				   = 2 tabular atmosphere from WEATHER_DECK
//
//				   mturb = 0 no turbulence
//						 = 1 dryden turbulence model
//
//						 mwind = 0 no wind
//							   = 1 constant wind, input: dvaeg,psiwdx
//      	   	               = 2 tabular wind from WEATHER_DECK
//
//030507 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////

void Round6::def_environment()
{
	//Definition and initialization of module-variables
	round6[1].init("press", 0, "Atmospheric pressure - Pa", "environment", "out", "");
	round6[2].init("rho", 0, "Atmospheric density - kg/m^3", "environment", "out", "");
	round6[3].init("vmach", 0, "Mach number of hyper - ND", "environment", "out", "");
	round6[4].init("pdynmc", 0, "Dynamic pressure - Pa", "environment", "out", "");
	round6[5].init("tempk", 0, "Atmospheric temperature - K", "environment", "out", "");
	round6[6].init("GRAVG", 0, 0, 0, "Grav accel in geocentric coord - m/s^2", "environment", "out", "");
	round6[7].init("psiwdx", 0, "Wind direction from north - m/s", "environment", "data", "");
	round6[8].init("VAEDS", 0, 0, 0, "Smoothed wind velocity in geodetic coord - m/s", "environment", "state", "");
	round6[9].init("VAED", 0, 0, 0, "Wind velocity in geodetic coordinates - m/s", "environment", "out", "");
	round6[10].init("dvba", 0, "Vehicle speed wrt air - m/s", "environment", "out", "");
	round6[50].init("tempc", 0, "Atmospheric temperature - Centigrade", "environment", "diag", "");
	round6[51].init("VAEDSD", 0, 0, 0, "Smoothed wind velocity derivative - m/s", "environment", "state", "");
	round6[52].init("mair", "int", 0, "mair =|matmo|mturb|mwind|", "environment", "data", "");
	round6[53].init("dvae", 0, "Magnitude of constant air speed - m/s", "environment", "data", "");
	round6[54].init("vaed3", 0, "Vertical air speed (pos.down) - m/s", "environment", "data", "");
	round6[55].init("twind", 0.1, "Wind smoothing time constant - sec", "environment", "data", "");


}	

///////////////////////////////////////////////////////////////////////////////
//Environment module initialiation
//Member function of class 'Round6'
//
// (1) Initializes airspeed dvba with geographic speed dvbe
//
//030528 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////

void Round6::init_environment()
{
	//local module-variables
	double dvba(0);

	//localizing module-variables
	//input from other modules
	double dvbe=round6[35].real();
//-----------------------------------------------------------------------------
	dvba=dvbe;
//-----------------------------------------------------------------------------
	//loading module-variables
	//initialization
	round6[10].gets(dvba);
}

///////////////////////////////////////////////////////////////////////////////
//Environment module
//Member function of class 'Round6'
//
// (1) Calculates the atmospheric properties
//
// The switch 'mair' controls the atmosphere, wind and air turbulence options:
//
//     mair=|matmo|mturb|mwind|
//
//		     matmo = 0 US 1976 Standard Atmosphere (public domain shareware)
//				   = 1 US 1976 Standard Atmosphere with extension up to 1000 km (NASA Marshall)
//				   = 2 tabular atmosphere from WEATHER_DECK
//
//				   mturb = 0 no turbulence
//						 = 1 dryden turbulence model
//
//						 mwind = 0 no wind
//							   = 1 constant wind, input: dvaeg,psiwdx
//      	   	               = 2 tabular wind from WEATHER_DECK
//
// (2) Tabular atmosphere in from WEATHER_DECK with density 'rho' in kg/m^3
//		pressure 'press' in Pa, and temperature in 'tempc' in deg Centigrade,
//		as a function of altitude 'alt' in m
// (3) Constant horizontal wind is input by 'dvae' and wind direction 'psiwdx'
//	   Tabular wind is from WEATHER_DECK with heading from north 'psiwdx' in deg
//		 and magnitude 'dvw' in m/s as a function of altitude 'alt' in m
// (4) Calculates the vehicles's Mach number and dynamic pressure
// (5) Heat equilibrium calculations on nose of vehicle
// (6) Gravitational acceleration based on WGS84 ellipsoid
//
//030507 Created by Peter H Zipfel
//040311 Added US76 Atmosphere extended to 1000km (NASA Marshall), PZi
//091216 Added tabular atmosphere and wind, PZi
///////////////////////////////////////////////////////////////////////////////

void Round6::environment(double int_step)
{
	//local variables
	double tempc(0);
	double tempk(0);
	double dvw(0);  
	//local module-variables
	double press(0);
	double rho(0);
	double vsound(0);
	double vmach(0);
	double pdynmc(0);
	Matrix GRAVG(3,1);
	Matrix VAED(3,1);
	//localizing module-variables
	//input data
	int mair=round6[52].integer();
	double dvae=round6[53].real(); 
	double vaed3=round6[54].real(); 
	double psiwdx=round6[7].real(); 
	double twind=round6[55].real(); 
	//getting saved values
	double dvba = round6[10].real();
	//state variables
	Matrix VAEDS=round6[8].vec();
	Matrix VAEDSD=round6[51].vec();
	//input from other modules
	double time=round6[0].real(); 
	double alt=round6[32].real();
	Matrix VBED=round6[41].vec();
	Matrix SBII=round6[42].vec();	
	//-------------------------------------------------------------------------
	//decoding the air switch
	int matmo=mair/100;
	//int mturb=(mair-matmo*100)/10;
	int mwind=(mair-matmo*100)%10;

	//gravitational acceleration in geocentric coordinates
	GRAVG=cad_grav84(SBII,time);

	//US 1976 Standard Atmosphere (public domain)
	if(matmo==0){
		atmosphere76(rho,press,tempk, alt);
		tempc=tempk-273.16;
		//speed of sound
		vsound=sqrt(1.4*RGAS*tempk);
	}

	//tabular atmosphere from WEATHER_DECK
	if(matmo==2){
		rho=weathertable.look_up("density",alt);
		press=weathertable.look_up("pressure",alt);
		tempc=weathertable.look_up("temperature",alt);
		//speed of sound
		tempk=tempc+273.16;
		vsound=sqrt(1.4*RGAS*tempk);
	}
	
	//wind options
	
	if(mwind>0){
		if(mwind==1)
			//constant wind
			dvw=dvae;

		if(mwind==2){
			//tabular wind from WEATHER_DECK
			dvw=weathertable.look_up("speed",alt);
			psiwdx=weathertable.look_up("direction",alt);
		}
		//wind components in geodetic coordinates
		Matrix VAED_RAW(3,1);
		VAED_RAW[0]=-dvw*cos(psiwdx*RAD);
		VAED_RAW[1]=-dvw*sin(psiwdx*RAD);
		VAED_RAW[2]=vaed3;

		//smoothing wind by filtering with time constant 'twind' sec
		Matrix VAEDSD_NEW=(VAED_RAW-VAEDS)*(1/twind);
		VAEDS=integrate(VAEDSD_NEW,VAEDSD,VAEDS,int_step);
		VAEDSD=VAEDSD_NEW;
		VAED=VAEDS;
	}
	//flight conditions
	Matrix VBAD=VBED-VAED;
	dvba=VBAD.absolute();

	//Mach number
	vmach=fabs(dvba/vsound);
	//Dynamic pressure
	pdynmc=0.5*rho*dvba*dvba;

	//loading module-variables
	//state variables
	round6[8].gets_vec(VAEDS);
	round6[51].gets_vec(VAEDSD);		
	//output to other modules
	round6[1].gets(press);
	round6[2].gets(rho);
	round6[3].gets(vmach);
	round6[4].gets(pdynmc);
	round6[6].gets_vec(GRAVG);
	round6[9].gets_vec(VAED);
	round6[10].gets(dvba);
	//diagnostics
	round6[5].gets(tempk);
	round6[50].gets(tempc);
}	
