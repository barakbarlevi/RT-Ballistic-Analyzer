///////////////////////////////////////////////////////////////////////////////
//FILE: 'intercept.cpp'
//Contains 'inercept' module of class 'Hyper'
//
//030619 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////

#include "class_hierarchy.hpp"

///////////////////////////////////////////////////////////////////////////////
//Definition of intercept module-variables
//Member function of class 'Hyper'
//Module-variable locations are assigned to hyper[650-699]
//
//040619 Created by Peter H Zipfel
//091214 Modified for ROCKET6, PZi
///////////////////////////////////////////////////////////////////////////////

void Hyper::def_intercept()
{
}

///////////////////////////////////////////////////////////////////////////////
//Intercept module
//Member function of class 'Hyper'
//Module-variable locations are assigned to hyper[650-699]
//
//Termination if impact on ground occurs
//
//030619 Created by Peter H Zipfel
//091214 Modified for ROCKET6, PZi
///////////////////////////////////////////////////////////////////////////////

void Hyper::intercept(int num_vehicles,int vehicle_slot,double int_step,const char * title, int* phealth)
{
	(void) int_step; // xxxx
	(void) title; // xxxx
	(void) vehicle_slot; // xxxx
	(void) num_vehicles; // xxxx
	double time=round6[0].real();
	(void) time; // xxxx
	double alt=round6[32].real();
	double dvbe=round6[35].real();
	(void) dvbe; // xxxx
	double psivdx=round6[37].real();
	(void) psivdx; // xxxx
	double thtvdx=round6[38].real();
	(void) thtvdx; // xxxx
	double thrust = hyper[3].real();

	//ground impact
	if ((alt <= 0) && (thrust == 0))
	{
		//cout<<"\n"<<" ***"<<title<<"   "<< __DATE__ <<" "<< __TIME__ <<" ***";
		//cout<<"      speed = "<<dvbe<<" m/s  heading = "<<psivdx<<" deg      gamma = "<<thtvdx<<" deg\n\n";    
		cout<<"***Ground impact " << __DATE__ <<" "<< __TIME__ <<" ***" << std::endl;
		*phealth = 0;
	}

}
