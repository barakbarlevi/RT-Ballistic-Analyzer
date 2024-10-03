///////////////////////////////////////////////////////////////////////////////
//FILE: 'aerodynamics.cpp'
//
//Contains 'aerodynamics' module of class 'Hyper'
//
//050103 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////

#include "class_hierarchy.hpp"

///////////////////////////////////////////////////////////////////////////////
//Definition of aerodynamic module-variables 
//Member function of class 'Hyper'
//Module-variable locations are assigned to hyper[100-199]
//
// This module performs the following functions:
// (1) Calculates the aerodynamic force and moment coefficients from the aero_deck
// (2) Derives the dimensional derivatives for the flight controllers
//
// maero = 0 no aerodynamic forces
//		   11 Booster - 1 stage (last stage)
//         12 Booster - 2 stages
//         13 Booster - 3 stages (launch)
//
//050103 Created by Peter H Zipfel
//091214 Modified for ROCKET6, PZi
///////////////////////////////////////////////////////////////////////////////

void Hyper::def_aerodynamics()
{
	hyper[25].init("maero", "int", 0, "=11: last stage; =12: 2 stages; =13: 3 stages", "aerodynamics", "data", "");
	hyper[26].init("refa", 0, "Reference area for aero coefficients - m^2", "aerodynamics", "init", "");
	hyper[27].init("refd", 0, "Reference length for aero coefficients - m", "aerodynamics", "init", "");
	hyper[28].init("xcg_ref", 0, "Reference cg location from nose - m", "aerodynamics", "init", "");

	//main force and moment coefficients
	hyper[5].init("cy", 0, "Side force coefficient - ND", "aerodynamics", "out", "");
	hyper[6].init("cll", 0, "Rolling moment coefficient - ND", "aerodynamics", "out", "");
	hyper[7].init("clm", 0, "Pitching moment coefficient - ND", "aerodynamics", "out", "");
	hyper[8].init("cln", 0, "Yawing moment coefficient - ND", "aerodynamics", "out", "");
	hyper[9].init("cx", 0, "X-force coefficient - ND", "aerodynamics", "out", "");
	hyper[10].init("cz", 0, "Z-force coefficient - ND", "aerodynamics", "out", "");
	//table look-up coefficients 
	hyper[11].init("ca0", 0, "Axial force coeff(Mach) - ND", "aerodynamics", "diag", "");
	hyper[12].init("caa", 0, "Delta axial force due to alpha(Mach) - ND", "aerodynamics", "diag", "");
	hyper[13].init("cn0", 0, "Normal force coeff(Mach,alpha) - ND", "aerodynamics", "diag", "");
	hyper[14].init("clm0", 0, "Pitch moment coeff(Mach,alpha) - ND", "aerodynamics", "diag", "");
	hyper[15].init("clmq", 0, "Pitch dampning deriv(Mach) - 1/deg", "aerodynamics", "diag", "");
	hyper[16].init("cla", 0, "Lift slope derivative(alpha,mach) - 1/deg", "aerodynamics", "out", "");
	hyper[17].init("cyb", 0, "Weather vane der wrt beta(alpha,mach) - 1/deg", "aerodynamics", "diag", "");
	hyper[18].init("cllp", 0, "Roll damping deriv(alpha,mach) - 1/rad", "aerodynamics", "diag", "");
	hyper[19].init("cma", 0, "Pitch moment due to alpha deriv(alpha,mach) -1/deg", "aerodynamics", "diag", "");
	hyper[20].init("cmq", 0, "Pitch dampning deriv(alpha,mach) - 1/rad", "aerodynamics", "diag", "");
	hyper[21].init("cnb", 0, "Yaw moment deriv(alpha,mach) - 1/deg", "aerodynamics", "diag", "");
	//calculated dimensional derivatives for autopilot
	hyper[22].init("stmarg_yaw", 0, "Static margin yaw (+stable, -unstable) - caliber", "aerodynamics", "diag", "");
	hyper[23].init("stmarg_pitch", 0, "Static margin pitch (+stable, -unstable) - caliber", "aerodynamics", "diag", "");
	//run termination conditions    
	hyper[24].init("trcode", 0, "Termination code number", "aerodynamics", "init", "");
	//limiters
	hyper[29].init("alplimx", 0, "Alpha limiter for vehicle - deg", "aerodynamics", "data", "");

	//hyper[42].init("clma", 0, "clma", "aerodynamics", "data", "");
	//hyper[43].init("cphip", 0, "cphip", "aerodynamics", "data", "");
	//hyper[44].init("clmaref", 0, "clmaref", "aerodynamics", "data", "");
	//hyper[45].init("cna", 0, "cna", "aerodynamics", "data", "");
	//hyper[46].init("secondterm", 0, "secondterm", "aerodynamics", "data", "");
}	
///////////////////////////////////////////////////////////////////////////////
//Initialization of aerodynamic module 
//Member function of class 'Hyper'
//Run stopping conditions
//
// maero = 0 no aerodynamic forces
//		   11 Booster - 1 stage (last stage)
//         12 Booster - 2 stages
//         13 Booster - 3 stages (launch)
//
//050103 Created by Peter H Zipfel
//091214 Modified for ROCKET6, PZi
//230505 Corrected aeo-table look-up for last stage, PZi
///////////////////////////////////////////////////////////////////////////////

void Hyper::init_aerodynamics()
{

	double trcode(0);
	trcode=0;

	//loading module-variables
	hyper[24].gets(trcode);

}
///////////////////////////////////////////////////////////////////////////////
//Aerodynamic module
//Member function of class 'Hyper'
//
//This module performs the following functions:
// (1) Aerodynamic table look-up from file 'aero_deck_SLV.asc'
// (2) Calculation of aero coefficients in body coordinates
//
//050103 Created by Peter H Zipfel
//091214 Modified for ROCKET6, PZi
//230425 Table look-up correction applied, PZi
///////////////////////////////////////////////////////////////////////////////

void Hyper::aerodynamics(double int_step)
{
	(void)int_step; // xxxx
	//local variable
	int thrust_on=false;
	//local module-variables
	double ca0b(0);
  	double ca(0);
	double clma(0);
	double cna(0);
	double cy(0);
	double cx(0);
	double cz(0);
	double cll(0);
	double clm(0);
	double cln(0);
	double cyb(0);
	double cllp(0);
	double cmq(0);
	double cnb(0);
	double ca0(0);
	double caa(0);
	double cn0(0);
	double clm0(0);
	double clmq(0);
	double secondterm(0);
	(void)secondterm; // xxxx


	//localizing module-variables
	//input data
	int maero=hyper[25].integer();
	(void)maero; // xxxx
	double refa=hyper[26].real();
	(void)refa; // xxxx
	double refd=hyper[27].real();
	double xcg_ref = hyper[28].real();
	double alplimx = hyper[29].real();
	//loading saved value
	double cla=hyper[16].real();
	double cma=hyper[19].real();
	//input from other modules
	double time=round6[0].real();
	(void)time; // xxxx
	double alppx=round6[17].real();
	double phipx=round6[18].real();
	double alphax=round6[19].real();
	(void)alphax; // xxxx
	double betax=round6[20].real();
	(void)betax; // xxxx
	double rho=round6[2].real();
	(void)rho; // xxxx
	double vmach=round6[3].real();
	double pdynmc=round6[4].real();
	(void)pdynmc; // xxxx
	double tempk=round6[5].real();
	(void)tempk; // xxxx
	double dvba=round6[10].real();
	double ppx=round6[21].real();
	(void)ppx; // xxxx
	double qqx=round6[22].real();
	double rrx=round6[23].real();
	double alt=round6[32].real();
	(void)alt; // xxxx
	double vmass=hyper[0].real();
	(void)vmass; // xxxx
	double xcg=hyper[1].real();
	double trcode=hyper[24].real();
	(void)trcode; // xxxx
	double thrust = hyper[3].real();
	//-------------------------------------------------------------------------
	//transforming body rates from body -> aeroballistic coord.
	double phip=phipx*RAD;
	double cphip=cos(phip);
	double sphip=sin(phip);
	double qqax=qqx*cphip-rrx*sphip;
	double rrax=qqx*sphip+rrx*cphip;
	(void)rrax; // xxxx

	//looking up axial force coefficients
	ca0=aerotable.look_up("ca0slv3_vs_mach",vmach);
	caa=aerotable.look_up("caaslv3_vs_mach",vmach);
	ca0b=aerotable.look_up("ca0bslv3_vs_mach",vmach);
	//axial force coefficient
	if(thrust>0) thrust_on=true;
	ca=ca0+caa*alppx+thrust_on*ca0b;

	//looking up normal force coefficients in aeroballistic coord
    cn0=aerotable.look_up("cn0slv3_vs_mach_alpha",vmach,alppx);
	//normal force coefficient
	cna=cn0;

	//looking up pitching moment coefficients in aeroballistic coord
	clm0=aerotable.look_up("clm0slv3_vs_mach_alpha",vmach,alppx);
	clmq=aerotable.look_up("clmqslv3_vs_mach",vmach);

	//pitching moment coefficient
	double clmaref=clm0+clmq*qqax*refd/(2.*dvba);

	clmaref = 0.6*clmaref;

	secondterm = -cna * (xcg_ref - xcg) / refd;
	clma=clmaref-cna*(xcg_ref-xcg)/refd;

	//clma = -0.005;
	//clma = -0.0025;
	clma = -0.01;

	//clma = clma * 0.8;

	double alplx(0),alpmx(0);
	double cn0p(0),cn0m(0);
	double clm0p(0),clm0m(0);

	//Non-dimensional derivatives
	//look up coeff at +- 3 deg, but not beyond tables
	alplx=alppx+3.0;
	alpmx=alppx-3.0;
    if(alpmx<0.)alpmx=0.0;

	//calculating normal force dim derivative wrt alpha 'cla'			
	cn0p=aerotable.look_up("cn0slv3_vs_mach_alpha",vmach,alplx);
	cn0m=aerotable.look_up("cn0slv3_vs_mach_alpha",vmach,alpmx);		
	
	//replacing value from previous cycle, only if within max alpha limit
	if(alplx<alplimx)
		cla=(cn0p-cn0m)/(alplx-alpmx);

	//calculating pitch moment dim derivative wrt alpha 'cma'
	clm0p=aerotable.look_up("clm0slv3_vs_mach_alpha",vmach,alplx);
	clm0m=aerotable.look_up("clm0slv3_vs_mach_alpha",vmach,alpmx);
	//replacing value from previous cycle, only if within max alpha limit
	if(alppx<alplimx)
		cma=(clm0p-clm0m)/(alplx-alpmx)-cla*(xcg_ref-xcg)/refd;

	//converting force and moment coeff to body axes
	//force coefficients in body axes
	cx=-ca;
	cy=-cna*sphip;
	cz=-cna*cphip;
	//moment coefficient in body axes
	cll=0;
	clm=clma*cphip;
	cln=-clma*sphip;

	//calculate load factor available for max alpha
	//looking up normal force coefficients in aeroballistic coord
	//converting output to be compatible with 'aerodynamics_der()'
	//force
	cyb=-cla;
	//roll
	cllp=0.0;
	//pitch
	cmq=clmq;
	//yaw
	cnb=-cma;

	//loading module-variables
	//output to other modules
	hyper[5].gets(cy);
	hyper[6].gets(cll);
	hyper[7].gets(clm);
	hyper[8].gets(cln);
	hyper[9].gets(cx);
	hyper[10].gets(cz);
	//output to 'aerodynamics_der()'
	hyper[16].gets(cla);
	hyper[17].gets(cyb);
	hyper[18].gets(cllp);
	hyper[19].gets(cma);
	hyper[20].gets(cmq);
	hyper[21].gets(cnb);
	//diagnostics
	hyper[11].gets(ca0);
	hyper[12].gets(caa);
	hyper[13].gets(cn0);
	hyper[14].gets(clm0);
	hyper[15].gets(clmq);

	//hyper[42].gets(clma);
	//hyper[43].gets(cphip);
	//hyper[44].gets(clmaref);
	//hyper[45].gets(cna);
	//hyper[46].gets(secondterm);


	//function call for derivative calculations
	aerodynamics_der();
}	

void Hyper::aerodynamics_der()
{
	//local module-variables
	double stmarg_pitch(0);
	double stmarg_yaw(0);
	//from aerodynamics module	
	double cma = hyper[19].real();
	double cla = hyper[16].real();
	double cnb = hyper[21].real();
	double cyb = hyper[17].real();
	//static margin in pitch (per chord length 'refd')
	if(cla) stmarg_pitch=-cma/cla;
	//static margin in yaw (per span length 'refd')
	if(cyb) stmarg_yaw=-cnb/cyb;
	//loading module-variables
	//output to other modules
	//diagnostics
	hyper[22].gets(stmarg_yaw);
	hyper[23].gets(stmarg_pitch);
}

