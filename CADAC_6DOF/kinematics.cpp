///////////////////////////////////////////////////////////////////////////////
//FILE: 'kinematics.cpp'
//Contains 'kinematics' module of class 'Round6'
//
//011126 Created from SRAAM6 by Peter H Zipfel
//030307 Upgraded to SM Item32, PZi
//050326 Moved 'time' calculations to kinematics module, PZi
///////////////////////////////////////////////////////////////////////////////

#include "class_hierarchy.hpp"

///////////////////////////////////////////////////////////////////////////////
//Definition of kinematics module-variables 
//Member function of class 'Round6'
//Module-variable locations are assigned to round6[100-149]
// 
//011126 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////

void Round6::def_kinematics()
{
	//Definition and initialization of module-variables
	round6[0].init("time",0,"Vehicle time since launch - s","kinematics","exec","plot");
	round6[11].init("TBD",0,0,0,0,0,0,0,0,0,"T.M. of body wrt geodetic coord","kinematics","out","");
    round6[12].init("TBI",0,0,0,0,0,0,0,0,0,"T.M. of body wrt inertial coord","kinematics","state","");
    round6[13].init("TBID",0,0,0,0,0,0,0,0,0,"T.M. of body wrt inertial coord derivative - 1/s","kinematics","state","");
    round6[14].init("psibdx",0,"Yawing angle of veh wrt geod coord - deg","kinematics","in/di","plot");
    round6[15].init("thtbdx",0,"Pitching angle of veh wrt geod coord - deg","kinematics","in/di","plot");
    round6[16].init("phibdx",0,"Rolling angle of veh wrt geod coord - deg","kinematics","in/di","");
    round6[17].init("alppx",0,"Total angle of attack - deg","kinematics","out","");
    round6[18].init("phipx",0,"Aerodynamic roll angle - deg","kinematics","out","");
    round6[19].init("alphax",0,"Angle of attack - deg","kinematics","init/diag","");
    round6[20].init("betax",0,"Side slip angle - deg","kinematics","diag","");
    
	//round6[57].init("tbd11",0,"TBD(1,1)","kinematics","out","plot");
	//round6[58].init("cthtbd", 0, "Cos(thtbdx)", "kinematics", "out", "plot");
	//round6[59].init("cpsi", 0, "Cos(psi)", "kinematics", "out", "plot");
	//round6[60].init("tbd12", 0, "TBD(1,2)", "kinematics", "out", "plot");

	//round6[61].init("vbab1", 0, "u", "kinematics", "out", "plot");
	//round6[62].init("vbab2", 0, "v", "kinematics", "out", "plot");
	//round6[63].init("vbab3", 0, "w", "kinematics", "out", "plot");
	//round6[64].init("udivV", 0, "udivV", "kinematics", "out", "plot");
	//round6[65].init("wdivu", 0, "wdivu", "kinematics", "out", "plot");

	

}	

///////////////////////////////////////////////////////////////////////////////
//Initialization of kinematics module
//Member function of class 'Round6'
//Setting 'sim_time' = 'time'
//Accepting new integration step size 'int_step' from 'input.asc'
//Initializes direction cosine matrix
// 
//011126 Created by Peter H Zipfel
//050326 Moved 'time' calculations to kinematics module, PZi
///////////////////////////////////////////////////////////////////////////////

void Round6::init_kinematics(double sim_time,double int_step)
{	
	(void)int_step; // xxxx
	//local module-variables
	double time(0);
	//double int_step_new(0);
	Matrix TBD(3,3);
	Matrix TBI(3,3);

	//localizing module-variables
	//input data
	double psibdx=round6[14].real();
	double thtbdx=round6[15].real();
	double phibdx=round6[16].real();
	//input from other modules
	double lonx=round6[30].real();  
	double latx=round6[31].real();  
	double alt=round6[32].real();   
	double dvbe=round6[35].real();   
	(void)dvbe; // xxxx
	//-------------------------------------------------------------------------
	//setting vehicle time to simulation time
	time=sim_time;
	//initializing the variable integration step
	//int_step_new=int_step;
	//-------------------------------------------------------------------------
	//initializing direction cosine matrix TBD, body wrt geodetic coord
	TBD=mat3tr(psibdx*RAD,thtbdx*RAD,phibdx*RAD);
	Matrix TDI=cad_tdi84(lonx*RAD,latx*RAD,alt,time);
	TBI=TBD*TDI;
	//-------------------------------------------------------------------------
	//loading module-variables
	//executive 
	round6[0].gets(time);
	//round6[2].gets(int_step_new);
	//initialized output
	round6[11].gets_mat(TBD);
	round6[12].gets_mat(TBI);
}
///////////////////////////////////////////////////////////////////////////////
//Kinematics module
//Member function of class 'Round6'
//Setting 'sim_time' = 'time'
//Saving 'event_time' of 'this' vehicle object
//Accepting new integration step size 'int_step' from 'input.asc'
//Changing all output step sizes by a factor  
// Solves the direction cosine differential equations
// Calculates Euler angles
// Calculates incidence angles
//
//011127 Created by Peter Zipfel
//050326 Moved 'time' calculations from newton to kinematics module, PZi
//230510 Corrected coding of phip, PZi
///////////////////////////////////////////////////////////////////////////////

//void Round6::kinematics(double sim_time,double event_time,double &int_step,double &out_fact)
void Round6::kinematics(double sim_time, double& int_step)
{
	//local variables
	Matrix UNIT(3,3); UNIT.identity();
	double cthtbd(0);
	double phip(0);
	
	//local module-variables
	double time(0);
	//double ortho_error(0);
	double thtbd(0);
	double psibd(0);
	double phibd(0);
	double psibdx(0);
	double thtbdx(0);
	double phibdx(0);
	double alphax(0);
	double betax(0);
	Matrix TBD(3,3);
	double alppx(0);
	double phipx(0);

	//localizing module-variables
	//input from other modules
	double dvba=round6[10].real();
	Matrix WBIB=round6[25].vec();
	double lonx=round6[30].real();  
	double latx=round6[31].real();  
	double alt=round6[32].real();
	Matrix VBED = round6[41].vec();
	Matrix VAED=round6[9].vec();
	//Matrix VBII=round6[236].vec();
	//state variables
	Matrix TBI=round6[12].mat();
	Matrix TBID=round6[13].mat();
	//----------------------------------------------------------------------------
	//setting vehicle time to simulation time
	time=sim_time;
	
	double udivV(0);
	(void)udivV; // xxx
	double wdivu(0);
	(void)wdivu; // xxxx

	//----------------------------------------------------------------------------
	//*integrating direction cosine matrix
	Matrix TBID_NEW=~WBIB.skew_sym()*TBI;
	TBI=integrate(TBID_NEW,TBID,TBI,int_step);
	TBID=TBID_NEW;

	//orthonormalizing TBI
	Matrix EE=UNIT-TBI*~TBI;
	TBI=TBI+EE*TBI*0.5;

	//TBI orthogonality check
	//double e1=EE.get_loc(0,0);
	//double e2=EE.get_loc(1,1);
	//double e3=EE.get_loc(2,2);
	//ortho_error=sqrt(e1*e1+e2*e2+e3*e3);

	//Euler angles
	Matrix TDI=cad_tdi84(lonx*RAD,latx*RAD,alt,time);
	TBD=TBI*~TDI;
	double tbd13=TBD.get_loc(0,2);
	double tbd11=TBD.get_loc(0,0);
	double tbd33=TBD.get_loc(2,2);
	double tbd12=TBD.get_loc(0,1);
	double tbd23=TBD.get_loc(1,2);

	//*geodetic Euler angles
	//pitch angle: 'thtbd'
	//note: when |tbd13| >= 1, thtbd = +- pi/2, but cos(thtbd) is
	//		forced to be a small positive number to prevent division by zero
	if(fabs(tbd13)<1){
		thtbd=asin(-tbd13);
		cthtbd=cos(thtbd);
	}
	else{
		thtbd=PI/2*sign(-tbd13);
		cthtbd=EPS;
	}
	//yaw angle: 'psibd'
	double cpsi=tbd11/cthtbd;
	if(fabs(cpsi)>1)
		cpsi=1*sign(cpsi);
	psibd=acos(cpsi)*sign(tbd12);
	//psibd = atan2(tbd12 , tbd11);

	//roll angle: 'phibdc'
	double cphi=tbd33/cthtbd;
	if(fabs(cphi)>1)
		cphi=1*sign(cphi);
	phibd=acos(cphi)*sign(tbd23);

	psibdx=DEG*psibd;
	thtbdx=DEG*thtbd;
	phibdx=DEG*phibd;

	//*incidence angles using wind vector VAED in geodetic coord
	Matrix VBAB=TBD*(VBED-VAED);
	double vbab1=VBAB.get_loc(0,0);
	double vbab2=VBAB.get_loc(1,0);
	double vbab3=VBAB.get_loc(2,0);

	wdivu = vbab3 / vbab1;

	double alpha=atan2(vbab3,vbab1);
	double beta=asin(vbab2/dvba);
	alphax=alpha*DEG;
	betax=beta*DEG;
	
	//incidence angles in load factor plane (aeroballistic)
	double dum=vbab1/dvba;
	udivV = dum;
	if(fabs(dum)>1)
		dum=1*sign(dum);
	double alpp=acos(dum);

	if(vbab2==0&&vbab3==0)
		phip=0.;
	//note: if vbeb2 is <EPS the value phip is forced to be 0 or PI
	//		to prevent oscillations
	else if(fabs(vbab2)<EPS)
		{if(vbab3>0) phip=0;
		if(vbab3<0) phip=PI;} //PZi: 230511 Included the two statements in {}
	else
		phip=atan2(vbab2,vbab3);
	alppx=alpp*DEG;
	phipx=phip*DEG;

	//loading module-variables
	//state variables
	round6[12].gets_mat(TBI);
	round6[13].gets_mat(TBID);
	//output to other modules
	round6[0].gets(time);
	round6[11].gets_mat(TBD);
	round6[14].gets(psibdx);
	round6[15].gets(thtbdx);
	round6[16].gets(phibdx);
	round6[17].gets(alppx);
	round6[18].gets(phipx);
	round6[19].gets(alphax);
	round6[20].gets(betax);

	//round6[57].gets(tbd11);
	//round6[58].gets(cthtbd);
	//round6[59].gets(cpsi);
	//round6[60].gets(tbd12);


	//round6[61].gets(vbab1);
	//round6[62].gets(vbab2);
	//round6[63].gets(vbab3);
	//round6[64].gets(udivV);
	//round6[65].gets(wdivu);


}