///////////////////////////////////////////////////////////////////////////////
//FILE: 'propulsion.cpp'
//Contains 'propulsion' module of class 'Round6'
//
//030514 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////

#include "class_hierarchy.hpp"

///////////////////////////////////////////////////////////////////////////////
//Definition of 'propulsion' module-variables 
//Member function of class 'Hyper'
//Module-variable locations are assigned to round6[10-49]
//
// mrpop = 0 no thrusting
//       = 3 constant thrust rocket (input)
//       = 4 constant thrust rocket under LTG control (set in 'guidance' module)
//
//030514 Created by Peter H Zipfel
//040302 Added rocket propulsion, PZi
//041009 Included 'mfreeze' capability, PZi
//050105 Modified for GSWS6, PZi
//060120 Modified for ascent booster, PZi
//091214 Modified for ROCKET6, PZi
///////////////////////////////////////////////////////////////////////////////

void Hyper::def_propulsion()
{
	//Definition and initialization of module-variables
    hyper[0].init("vmass",0,"Vehicle mass - kg","propulsion","out","");
    hyper[1].init("xcg",0,"CG location from nose (pos) - m","propulsion","out","");
    hyper[2].init("IBBB",0,0,0,0,0,0,0,0,0,"Vehicle moment of inertia - kgm^2","propulsion","out","");
    hyper[3].init("thrust",0,"Thrust - N","propulsion","out","plot");
    hyper[4].init("fmassr",0,"Remaining fuel mass - kg","propulsion","save",""); 
	hyper[30].init("fmasse", 0, "Fuel mass expended (zero initialization required) - kg", "propulsion", "state", "");
	hyper[31].init("fmassd", 0, "Fuel mass expended derivative - kg/s", "propulsion", "state", "");
	hyper[34].init("spi", 0, "Specific impulse - sec", "propulsion", "data", "");
	hyper[32].init("vmass0", 0, "Initial gross mass - kg", "propulsion", "data", "");
	hyper[33].init("fmass0", 0, "Initial fuel mass in stage - kg", "propulsion", "data", "");
	hyper[35].init("xcg_0", 0, "Initial cg location from nose - m", "propulsion", "data", "");
	hyper[36].init("xcg_1", 0, "Final cg location from nose - m", "propulsion", "data", "");
	hyper[37].init("fuel_flow_rate", 0, "Fuel flow rate of rocket motor - kg/s", "propulsion", "data", "");
	hyper[38].init("moi_roll_0", 0, "Roll MOI of vehicle, initial - kgm^2", "propulsion", "data", "");
	hyper[39].init("moi_roll_1", 0, "Roll MOI of vehicle, burn-out - kgm^2", "propulsion", "data", "");
	hyper[40].init("moi_trans_0", 0, "Transverse MOI of vehicle, initial - kgm^2", "propulsion", "data", "");
	hyper[41].init("moi_trans_1", 0, "Transverse MOI of vehicle, burn-out - kgm^2", "propulsion", "data", "");
   
}	
///////////////////////////////////////////////////////////////////////////////
//Propulsion initialization module
//Member function of class 'Hyper'
// Initializes mass properties
//
//030514 Created by Peter H Zipfel
//040302 Added rocket propulsion, PZi
///////////////////////////////////////////////////////////////////////////////

void Hyper::init_propulsion()
{
//placeholder
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//Propulsion module
//Member function of class 'Hyper'
// Calculates engine thrust
// Provides dynamic pressure controller for hypersonic propulsion
//
// mrpop = 0 no thrusting
//       = 3 constant thrust rocket (input)
//       = 4 constant thrust rocket under LTG control (set in 'guidance' module)
//
// Note, vaccum thrust specifications are used from launch until insertion,
//       no backpressure corrections 
// 
//030514 Created by Peter H Zipfel
//040302 Added rocket propulsion, PZi
//050502 Added provisions for discrete change of MOI, PZi
//091214 Modified for ROCKET6, PZi
///////////////////////////////////////////////////////////////////////////////
void Hyper::propulsion(double int_step)
{
	double thrust = hyper[3].real();
	double fmassr = hyper[4].real();
	//state variable
	double fmasse = hyper[30].real();
	double fmassd = hyper[31].real();
	//input data
	double vmass0 = hyper[32].real();
	double fmass0 = hyper[33].real();
	double spi = hyper[34].real();
	double vmass = hyper[0].real();

	//calculating fuel consumption
	double fmassd_next = thrust / (spi * AGRAV);
	fmasse = integrate(fmassd_next, fmassd, fmasse, int_step);
	fmassd = fmassd_next;

	//calculating vehicle mass, mass flow, and fuel mass remaining
	vmass = vmass0 - fmasse;
	fmassr = fmass0 - fmasse;

	if (fmassr <= 0) {
		thrust = 0;
	}
	else {
		Matrix IBBB0(3,3);
		Matrix IBBB1(3,3);
		double xcg_0=hyper[35].real();
		double xcg_1=hyper[36].real();
		double fuel_flow_rate=hyper[37].real();
		double moi_roll_0=hyper[38].real();
		double moi_roll_1=hyper[39].real();
		double moi_trans_0=hyper[40].real();
		double moi_trans_1=hyper[41].real();
		double xcg=hyper[1].real();
		Matrix IBBB=hyper[2].mat();				 
		
		//constant thrust rocket engine for booster
		thrust = spi * fuel_flow_rate * AGRAV;
		
		//load MOI of booster
		IBBB0.zero();
		IBBB0.assign_loc(0, 0, moi_roll_0);
		IBBB0.assign_loc(1, 1, moi_trans_0);
		IBBB0.assign_loc(2, 2, moi_trans_0);
		IBBB1.zero();
		IBBB1.assign_loc(0, 0, moi_roll_1);
		IBBB1.assign_loc(1, 1, moi_trans_1);
		IBBB1.assign_loc(2, 2, moi_trans_1);

		//interpolating moment of inertia tensor as a function of fuel expended
		double mass_ratio = fmasse / fmass0;
		IBBB = IBBB0 + (IBBB1 - IBBB0) * mass_ratio;

		//interpolating cg as a function of fuel expended
		xcg = xcg_0 + (xcg_1 - xcg_0) * mass_ratio;

		//loading module-variables
		//state variables
		hyper[30].gets(fmasse);
		hyper[31].gets(fmassd);
		//saving variables
		hyper[4].gets(fmassr);
		//output to other modules
		hyper[0].gets(vmass);
		hyper[1].gets(xcg);
		hyper[2].gets_mat(IBBB);
	}
	hyper[3].gets(thrust);
}
