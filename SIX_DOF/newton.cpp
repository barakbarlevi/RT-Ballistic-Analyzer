///////////////////////////////////////////////////////////////////////////////
//FILE: 'newton.cpp'
//Contains 'newton' module of class 'Round6'
//
//030416 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////

#include "class_hierarchy.hpp"

///////////////////////////////////////////////////////////////////////////////
//Definition of newton module-variables 
//Member function of class 'Round6'
//Module-variable locations are assigned to round6[210-299]
// 
//Initializing variables for the newton module.
//
//030410 Created by Peter H Zipfel
//050222 Variable integration step size, PZi
///////////////////////////////////////////////////////////////////////////////

void Round6::def_newton()
{
	//Definition of module-variables
	round6[30].init("lonx",0,"Vehicle longitude - deg","newton","init,diag","plot");
	round6[31].init("latx",0,"Vehicle latitude - deg","newton","init,diag","plot");
	round6[32].init("alt",0,"Vehicle altitude - m","newton","init,out","plot");
	round6[33].init("TVD",0,0,0,0,0,0,0,0,0,"TM of geo velocity wrt geodetic coord - ND ","newton","out","");
	round6[34].init("TDI",0,0,0,0,0,0,0,0,0,"TM of geodetic wrt inertial  coordinates ","newton","init","");
	round6[35].init("dvbe",0,"Vehicle geographic speed - m/s","newton","init,out","plot");
	round6[36].init("dvbi",0,"Vehicle inertial speed - m/s","newton","out","");
	round6[37].init("psivdx",0,"Vehicle heading angle - deg","newton","init,out","plot");
	round6[38].init("thtvdx",0,"Vehicle flight path angle - deg","newton","init,out","plot");
	round6[39].init("dbi",0,"Vehicle distance from center of Earth - m","newton","out","");
	round6[40].init("TGI",0,0,0,0,0,0,0,0,0,"TM of geocentric wrt inertial  coordinates ","newton","init","");
	round6[41].init("VBED",0,0,0,"Geographic velocity in geodetic coord - m/s ","newton","out","plot");
	round6[42].init("SBII",0,0,0,"Inertial position - m ","newton","state","");
	round6[43].init("VBII",0,0,0,"Inertial velocity - m/s ","newton","state","");
	round6[44].init("ABII",0,0,0,"Inertial acceleration - m/s^2 ","newton","save","");
	round6[45].init("grndtrck",0,"Vehicle ground track on Earth surface - m","newton","diag","plot");
	round6[46].init("FSPB",0,0,0,"Specifiv force in body coord - m/s^2 ","newton","out","");
	round6[47].init("ayx",0,"Achieved side acceleration - g's","newton","diag","");
	round6[48].init("anx",0,"Achieved normal acceleration - g's","newton","diag","");
	round6[49].init("axx",0,"Achieved longitudinal acceleration - g's","newton","diag","");
	round6[56].init("WEII", 0, 0, 0, 0, 0, 0, 0, 0, 0, "Earth's angular velocity (skew-sym) - rad/s ", "newton", "init", "");
}

///////////////////////////////////////////////////////////////////////////////
//Initial calculations of newton module 
//Member function of class 'Round6'
// 
//Initial calculations.
// 
//030410 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////

void Round6::init_newton()
{
	//local variables
	Matrix VBEB(3,1);
	Matrix POLAR(3,1);

	//local module-variables
	//Matrix WBII(3,3);
	double dbi(0);
	Matrix VBED(3,1);
	Matrix SBII(3,1);
	Matrix VBII(3,1);
	double dvbi(0);
	double psivdx(0);
	double thtvdx(0);

	//localizing module-variables
	//input data
	double dvbe=round6[35].real();  
	double lonx=round6[30].real();  
	double latx=round6[31].real();  
	double alt=round6[32].real();
	//input from other modules
	double time=round6[0].real();
	double psibdx=round6[14].real();
	double thtbdx=round6[15].real();
	double phibdx=round6[16].real();
	double alpha0x(0);	
	double beta0x=(0);	
	//-----------------------------------------------------------------------------
	//Earth's angular velocity skew-symmetric matrix (3x3)
	Matrix WEII(3,3);
	WEII.assign_loc(0,1,-WEII3);
	WEII.assign_loc(1,0,WEII3);

	//converting geodetic lonx, latx, alt to SBII
	SBII=cad_in_geo84(lonx*RAD,latx*RAD,alt,time);
	dbi=SBII.absolute();
			
	//building geodetic velocity VBED(3x1) from  alpha, beta, and dvbe 
    //double salp=sin(alpha0x*RAD);
    //double calp=cos(alpha0x*RAD);
	double salp(0);
	double calp(1);
    double sbet=sin(beta0x*RAD);
    double cbet=cos(beta0x*RAD);
    double vbeb1=calp*cbet*dvbe;
    double vbeb2=sbet*dvbe;
    double vbeb3=salp*cbet*dvbe;
	VBEB.build_vec3(vbeb1,vbeb2,vbeb3);

	//building TBD 
	Matrix TBD=mat3tr(psibdx*RAD,thtbdx*RAD,phibdx*RAD); 
	//Geodetic velocity
	VBED=~TBD*VBEB;

	//building inertial velocity
	Matrix TDI=cad_tdi84(lonx*RAD,latx*RAD,alt,time);
	Matrix TGI=cad_tgi84(lonx*RAD,latx*RAD,alt,time);
	VBII=~TDI*VBED+WEII*SBII;
	dvbi=VBII.absolute();

	//calculating geodetic flight path angles (plotting initialization)
	POLAR=VBED.pol_from_cart();
	psivdx=DEG*POLAR.get_loc(1,0);
	thtvdx=DEG*POLAR.get_loc(2,0);
	//-----------------------------------------------------------------------------
	//loading module-variables
	round6[30].gets(lonx);
	round6[31].gets(latx);
	round6[34].gets_mat(TDI);
	round6[36].gets(dvbi);
	round6[56].gets_mat(WEII);
	round6[37].gets(psivdx);
	round6[38].gets(thtvdx);
	round6[39].gets(dbi);
	round6[40].gets_mat(TGI);
	round6[41].gets_vec(VBED);
	round6[42].gets_vec(SBII);
	round6[43].gets_vec(VBII);
	//output to other module
	round6[14].gets(psibdx);
}
///////////////////////////////////////////////////////////////////////////////
//Newton module
//Member function of class 'Round6'
//
//Solving the translational equations of motion using Newton's 2nd Law
//
//030410 Created by Peter H Zipfel
//050222 Variable integration step size, PZi
///////////////////////////////////////////////////////////////////////////////

void Round6::newton(double int_step)
{
	//local variables
	double lon(0);
	double lat(0);

	//local module-variables
	double dvbe(0);
	double dvbi(0);
	double lonx(0);
	double latx(0);
	double alt(0);
	double altx(0);
	double psivdx(0);
	double thtvdx(0);
	Matrix FSPB(3,1);
	double axx(0);
	double anx(0);
	double ayx(0);
	double dbi(0);
	double gndtrkmx(0);
	double gndtrnmx(0);
	Matrix TVD(3,3);
	Matrix VBED(3,1);
	
	//localizing module-variables
	//initializations
	Matrix TDI=round6[34].mat();
	Matrix TGI=round6[40].mat();
	Matrix WEII=round6[56].mat();
	//input data
	//getting saved values
	double grndtrck=round6[45].real();
	//state variables
	Matrix SBII=round6[42].vec();
	Matrix VBII=round6[43].vec();
	Matrix ABII=round6[44].vec();
	//input from other modules
	double time=round6[0].real();
	Matrix GRAVG=round6[6].vec();
	Matrix TBI=round6[12].mat();	    
	Matrix FAPB=round6[28].vec();  	
	double vmass=hyper[0].real();
	//-----------------------------------------------------------------------------
	//integrating vehicle acceleration
	FSPB=FAPB*(1./vmass);
	Matrix NEXT_ACC=~TBI*FSPB+~TGI*GRAVG;
	Matrix NEXT_VEL=integrate(NEXT_ACC,ABII,VBII,int_step);
	SBII=integrate(NEXT_VEL,VBII,SBII,int_step);
	ABII=NEXT_ACC;
	VBII=NEXT_VEL;
	dvbi=VBII.absolute();
	dbi=SBII.absolute();

	//geodetic longitude, latitude and altitude
	cad_geo84_in(lon,lat,alt,SBII,time);
	TDI=cad_tdi84(lon,lat,alt,time);
	TGI=cad_tgi84(lon,lat,alt,time);
	lonx=lon*DEG;
	latx=lat*DEG;
	//altitude in kft for diagnostics
	altx=0.001*alt*FOOT;

	//geographic velocity in geodetic axes VBED(3x1) and flight path angles
	VBED=TDI*(VBII-WEII*SBII);
	Matrix POLAR=VBED.pol_from_cart();
	dvbe=POLAR[0];
	psivdx=DEG*POLAR[1];
	thtvdx=DEG*POLAR[2];

	//T.M. of geographic velocity wrt geodetic coordinates
	TVD=mat2tr(psivdx*RAD,thtvdx*RAD);

	//diagnostics: acceleration achieved
	axx=FSPB[0]/AGRAV;
	ayx=FSPB[1]/AGRAV;
	anx=-FSPB[2]/AGRAV;

	//ground track travelled (10% accuracy, usually on the high side)
	double vbed1=VBED[0];
	double vbed2=VBED[1];
	grndtrck+=sqrt(vbed1*vbed1+vbed2*vbed2)*int_step*REARTH/dbi;
	gndtrkmx=0.001*grndtrck;
	gndtrnmx=NMILES*grndtrck;

		//loading module-variables
	//state variables
	round6[42].gets_vec(SBII);
	round6[43].gets_vec(VBII);
	round6[44].gets_vec(ABII);
	//saving values
	round6[45].gets(grndtrck);
	//output to other modules
	round6[30].gets(lonx);
	round6[31].gets(latx);
	round6[32].gets(alt);
	round6[33].gets_mat(TVD);
	round6[34].gets_mat(TDI);
	round6[35].gets(dvbe);
	round6[36].gets(dvbi);
	round6[40].gets_mat(TGI);
	round6[41].gets_vec(VBED);
	round6[46].gets_vec(FSPB);
	//diagnostics
	round6[37].gets(psivdx);
	round6[38].gets(thtvdx);
	round6[39].gets(dbi);
	round6[47].gets(ayx);
	round6[48].gets(anx);
	round6[49].gets(axx);
}
