///////////////////////////////////////////////////////////////////////////////
//FILE: 'forces.cpp'
//Contains 'forces' module of class 'Hyper'
//
//030507 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////

#include "class_hierarchy.hpp"

///////////////////////////////////////////////////////////////////////////////
//Definition of forces module-variables 
//Member function of class 'Hyper'
//Module-variable locations are assigned to round6[200-209]
// 
//Note that FAPM and FMB is entered into the round6[] array because they are needed
// for the 'newton' and 'euler' modules, which are members of the 'Round6' class
//
//030507 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////

void Hyper::def_forces()
{
	//Definition and initialization of module-variables
    round6[28].init("FAPB",0,0,0,"Aerodynamic and propulsive forces in body axes - N","forces","out","");
    round6[29].init("FMB",0,0,0,"Aerodynamic and propulsive moments in body axes - N*m","forces","out","");
}	

///////////////////////////////////////////////////////////////////////////////
//Force module
//Member function of class 'Hyper'
// Calculates the non-gravitational forces
// Calculates the aerodynamic moments
//
//030507 Created by Peter H Zipfel
//051207 Modified for TVC, PZi
///////////////////////////////////////////////////////////////////////////////

void Hyper::forces()
{
	//local module-variables
	Matrix FAPB(3,1);
	Matrix FMB(3,1);

	//localizing module-variables
	//input from other modules
	double time=round6[0].real();
	double pdynmc=round6[4].real();
	double thrust=hyper[3].real();
	double refa=hyper[26].real();
    double refd=hyper[27].real();
	double cy=hyper[5].real();
	double cll=hyper[6].real();
	double clm=hyper[7].real();
	double cln=hyper[8].real();
	double cx=hyper[9].real();
	double cz=hyper[10].real();
	//-------------------------------------------------------------------------
	//total non-gravitational forces
	FAPB.assign_loc(0,0,pdynmc*refa*cx);
	FAPB.assign_loc(1,0,pdynmc*refa*cy);
	FAPB.assign_loc(2,0,pdynmc*refa*cz);

	//aerodynamic moment
	FMB.assign_loc(0,0,pdynmc*refa*refd*cll);
	FMB.assign_loc(1,0,pdynmc*refa*refd*clm);
	FMB.assign_loc(2,0,pdynmc*refa*refd*cln);

	if(thrust>0)
		FAPB[0]=FAPB[0]+thrust;

	//output to other modules
	round6[28].gets_vec(FAPB);
	round6[29].gets_vec(FMB);
}	
