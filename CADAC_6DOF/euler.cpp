///////////////////////////////////////////////////////////////////////////////
//FILE: 'euler.cpp'
//Contains 'euler' module of class 'Round6'
//
//030424 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////

#include "class_hierarchy.hpp"

///////////////////////////////////////////////////////////////////////////////
//Definition of 'euler' module-variables 
//Member function of class 'Round6'
//Module-variable locations are assigned to round6[150-199]
// 
//030424 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////

void Round6::def_euler()
{
	//Definition and initialization of module-variables
    round6[21].init("ppx",0,"Body roll angular vel wrt Earth in body axes - deg/s","euler","out","");
    round6[22].init("qqx",0,"Body pitch angular vel wrt Earth in body axes - deg/s","euler","out","");
    round6[23].init("rrx",0,"Body yaw angular vel wrt Earth in body axes - deg/s","euler","out","");
    round6[24].init("WBEB",0,0,0,"Ang vel of veh wrt Earth, body axes - rad/s","euler","diag","");
    round6[25].init("WBIB",0,0,0,"Ang vel of veh wrt inertial frame, body axes - rad/s","euler","state","");
    round6[26].init("WBIBD",0,0,0,"Ang vel of veh wrt inertl frame, deriv - rad/s^2","euler","state","");
    //round6[27].init("WBII",0,0,0,"Vehicle's inertial angular vel in inertial coord - rad/s^2","euler","out","");
}

///////////////////////////////////////////////////////////////////////////////
//Euler initialization module
//Member function of class 'Round6'
//
//030424 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////

void Round6::init_euler()
{
	//local variable 
	Matrix WEII(3,1);

	//local module-variables
	Matrix WBEB(3,1);
	Matrix WBIB(3,1);

	//localizing module-variables
	//input data
	double ppx=round6[21].real();
	double qqx=round6[22].real();
	double rrx=round6[23].real();
	//input from other modules
	Matrix TBI=round6[12].mat(); 
	//-------------------------------------------------------------------------
	//body rate wrt Earth frame in body coordinates
	WBEB.build_vec3(ppx*RAD,qqx*RAD,rrx*RAD);

	//body rate wrt ineritial frame in body coordinates
	WEII.build_vec3(0,0,WEII3);
	WBIB=WBEB+TBI*WEII;
	//-------------------------------------------------------------------------
	//loading module-variables
	//initialization
	round6[25].gets_vec(WBIB);
}
///////////////////////////////////////////////////////////////////////////////
//Euler module
//Member function of class 'Round6'
//
//030424 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////

void Round6::euler(double int_step)
{
	//local variable 
	Matrix WEII(3,1);

	//local module-variables
	double ppx(0),qqx(0),rrx(0);
	Matrix WBEB(3,1);
	//Matrix WBII(3,1);

	//localizing module-variables
	//input from other modules
	Matrix FMB=round6[29].vec();
	Matrix TBI=round6[12].mat(); 
	Matrix IBBB=hyper[2].mat();
	//state variable
	Matrix WBIB=round6[25].vec();
	Matrix WBIBD=round6[26].vec();
	//-------------------------------------------------------------------------
	//integrating the angular velocity acc wrt the inertial frame in body coord
	Matrix WACC_NEXT=IBBB.inverse()*(FMB-WBIB.skew_sym()*IBBB*WBIB);
	WBIB=integrate(WACC_NEXT,WBIBD,WBIB,int_step);
	WBIBD=WACC_NEXT;

	//angular velocity wrt inertial frame in inertial coordinates
	//WBII=~TBI*WBIB;
		
	//angular velocity wrt Earth in body coordinates
	WEII.build_vec3(0,0,WEII3);
	WBEB=WBIB-TBI*WEII;

	//body rates in deg/s
	ppx=WBEB.get_loc(0,0)*DEG;
	qqx=WBEB.get_loc(1,0)*DEG;
	rrx=WBEB.get_loc(2,0)*DEG;
	//-------------------------------------------------------------------------
	//loading module-variables
	//state variables
	round6[25].gets_vec(WBIB);	
	round6[26].gets_vec(WBIBD); 	
	//output to other modules
	round6[21].gets(ppx);	
	round6[22].gets(qqx);	
	round6[23].gets(rrx);
	round6[24].gets_vec(WBEB);
	//round6[27].gets_vec(WBII);
}