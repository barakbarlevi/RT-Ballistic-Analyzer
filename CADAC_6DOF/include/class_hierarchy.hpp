///////////////////////////////////////////////////////////////////////////////
//FILE: class_hierarchy.hpp
//
//Contains the classes of the hierarchy of base class 'Cadac'
//
//011128 Created by Peter H Zipfel
//030415 Adapted to HYPER simulation, PZi
//091216 Added WEATHER_DECK, PZI
//200320 Slightly modified for SSR6, PHZ
///////////////////////////////////////////////////////////////////////////////

#ifndef cadac_class_hierarchy__HPP
#define cadac_class_hierarchy__HPP

#include "global_header.hpp"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
//Abstract base class: Cadac
//
//011128 Created by Peter H Zipfel
//030415 Adapted to HYPER simulation, PZi
///////////////////////////////////////////////////////////////////////////////
class Cadac
{
private:
	 //vehicle object name
	char name[CHARN]="";
		
protected:
	//module-variable array of class 'Round6'
	//first 10 locations reserved for time and 'com' variables
	Variable *round6=nullptr;
	//Array of module-variables as defined in class 'Hyper'
	//first ten locations are reserved for 'com' variables
	Variable *hyper=nullptr;

public:
	//flag indicating an 'event' has occured
	//bool event_epoch=0;

	//time elapsed in event 
	//double event_time=0; //event_time

	virtual~Cadac(){};

	///////////////////////////////////////////////////////////////////////////
	//Constructor of class 'Cadac'
	//
	//010703 Created by Peter H Zipfel
	///////////////////////////////////////////////////////////////////////////
	Cadac(){}

	///////////////////////////////////////////////////////////////////////////
	//Setting vehicle object name
	//
	//010703 Created by Peter H Zipfel
	///////////////////////////////////////////////////////////////////////////
	void set_name(const char * vehicle_name) {strcpy(name,vehicle_name);}

	///////////////////////////////////////////////////////////////////////////
	//Getting vehicle object name
	//
	//010703 Created by Peter H Zipfel
	///////////////////////////////////////////////////////////////////////////
	const char * get_vname() {return name;}

	//////////////////////////executive functions /////////////////////////////
	virtual void sizing_arrays()=0;
	virtual void vehicle_array()=0;
	virtual void plot_array()=0;
	virtual void vehicle_data(fstream &input,int nmonte)=0;
	virtual void read_tables(const char * file_name,Datadeck &datatable)=0;
	virtual void plot_banner(ofstream &fplot,const char * title)=0;
	virtual void plot_index_arrays()=0;
	virtual void plot_data(ofstream &fplot,bool merge)=0;

	//module functions -MOD
	virtual void def_kinematics()=0;
	virtual void init_kinematics(double sim_time,double int_step)=0;
	//virtual void kinematics(double sim_time,double &int_step,double &out_fact)=0;
	virtual void kinematics(double sim_time, double& int_step) = 0;
	virtual void def_newton()=0;
	virtual void init_newton()=0;
	virtual void newton(double int_step)=0;
	virtual void init_euler()=0;
	virtual void def_euler()=0;
	virtual void euler(double int_step)=0;
	virtual void def_environment()=0;
	virtual void init_environment()=0;
	virtual void environment(double int_step)=0;
	virtual void def_aerodynamics()=0;
	virtual void init_aerodynamics()=0;
	virtual void aerodynamics(double int_step)=0; 
	virtual void def_forces()=0;
	virtual void forces()=0;
	virtual void def_propulsion()=0;
	virtual void init_propulsion()=0;
	virtual void propulsion(double int_step)=0;
	virtual void def_intercept()=0;
	virtual void intercept(int num_vehicles,int vehicle_slot,double int_step,const char * title, int* phealth)=0;
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//Derived class: Round6
//
//First derived class in the 'Cadac' hierarchy
//Models atmosphere, gravitaional acceleration and equations of motions
//Contains modules: 'environment', 'kinematics', 'newton', 'euler' 
//
//011128 Created by Peter H Zipfel
//030415 Adapted to HYPER simulation, PZi
///////////////////////////////////////////////////////////////////////////////
class Round6:public Cadac
{
protected:			
	//Indicator array pointing to the module-variable which are to 
	//be written to the screen
	int *round6_scrn_ind=nullptr;
	int round6_scrn_count=0;

	//Indicator-array pointing to the module-variable which are to 
	//be written to the 'ploti.asc' files
	int *round6_plot_ind=nullptr;
	int round6_plot_count=0;

	//Indicator-array pointing to the module-variable which are to 
	//be written to 'combus' 'packets'
	//int *round6_com_ind=nullptr;
	//int round6_com_count=0;

	//declaring Table pointer as temporary storage of a single table
	Table *table=nullptr;
	//declaring Datadeck 'weathertable' that stores all weather tables
	Datadeck weathertable;

public:
	Round6();
	virtual~Round6(){};

	//executive functions 
	virtual void sizing_arrays()=0;
	virtual void vehicle_array()=0;
	virtual void plot_array()=0;
	virtual void vehicle_data(fstream &input,int nmonte)=0;
	virtual void read_tables(const char * file_name,Datadeck &datatable)=0;
	virtual void plot_index_arrays()=0;
	virtual void plot_data(ofstream &fplot,bool merge)=0;

	//module functions -MOD
	virtual void def_aerodynamics()=0;
	virtual void init_aerodynamics()=0;
	virtual void aerodynamics(double int_step)=0; 
	virtual void def_forces()=0;
	virtual void forces()=0;
	virtual void def_propulsion()=0;
	virtual void init_propulsion()=0;
	virtual void propulsion(double int_step)=0;
	virtual void def_intercept()=0;
	virtual void intercept(int num_vehicles,int vehicle_slot,double int_step,const char * title, int* phealth)=0;

	//virtual functions to be declared in this class
	virtual void def_newton();
	virtual void init_newton();
	virtual void newton(double int_step);
	virtual void def_kinematics();
	virtual void init_kinematics(double sim_time,double int_step);
	virtual void kinematics(double sim_time, double& int_step);
	virtual void def_euler();
	virtual void init_euler();
	virtual void euler(double int_step);
	virtual void def_environment();
	virtual void init_environment();
	virtual void environment(double int_step);

};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//Derived class: Hyper
//
//Second level of derived class of the 'Cadac' hierarchy
//Models aerodynamics, propulsion, guidance and control 
//Contains Modules: 'aerodynamics', 'forces', 'propulsion', 'control',
// 'ins', 'guidance', 'gps', 'startrack', 'rcs' 
//
//011128 Created by Peter H Zipfel
//030415 Adapted to HYPER simulation, PZi
///////////////////////////////////////////////////////////////////////////////
class Hyper:public Round6
{
protected:
	//name of HYPER6 vehicle object
	char hyper6_name[CHARL];

	//Event list of 'Event' object pointers and actual number of events 
	//Event *event_ptr_list[NEVENT];int nevent;
	//total number of envents for a vehicle object
	//int event_total;

	//Compacted array of all module-variables of vehicle object 'Hyper'
	Variable *hyper6;int nhyper6;

	//Screen ouput array of module-variables of vehicle object 'Hyper'
	//Variable *scrn_hyper6;int nscrn_hyper6;

	//Plot ouput array of module-variables of vehicle object 'Hyper'
	Variable *plot_hyper6;int nplot_hyper6;

	//Indicator array pointing to the module-variable which are to 
	//be written to the 'ploti.asc' files
	int *hyper_plot_ind; int hyper_plot_count;

	//array of module-variables that carry Markov process random values
	//Markov markov_list[NMARKOV]; int nmarkov;

	//declaring Datadeck 'aerotable' that stores all aero tables
	Datadeck aerotable;
	//declaring Datadeck 'proptable' that stores all aero tables
	Datadeck proptable;

public:
	Hyper(){};
	Hyper(Module *module_list,int num_modules);
	virtual~Hyper();

	//executive functions
	virtual void sizing_arrays();
	virtual void vehicle_array();
	virtual void plot_array();
	virtual void vehicle_data(fstream &input,int nmonte);
	virtual void read_tables(const char * file_name,Datadeck &datatable);
	virtual void plot_banner(ofstream &fplot,const char * title);
	virtual void plot_index_arrays();
	virtual void plot_data(ofstream &fplot,bool merge);

	//module functions -MOD
	virtual void def_aerodynamics();
	virtual void init_aerodynamics();
	virtual void aerodynamics(double int_step); 
	virtual void def_forces();
	virtual void forces();
	virtual void def_propulsion();
	virtual void init_propulsion();
	virtual void propulsion(double int_step);
	virtual void def_intercept();
	virtual void intercept(int num_vehicles,int vehicle_slot,double int_step,const char * title, int* phealth);
	
	//functions in respective modules
	void aerodynamics_der();
};

///////////////////////////////////////////////////////////////////////////////
////////////////////////// Global class 'Vehicle'//////////////////////////////
///////////// must be located after 'Cadac' hierarchy in this file ////////////
///////////////////////////////////////////////////////////////////////////////
//Class 'Vehicle'
//
//Global class for typifying the array of vehicle pointers
//
//010629 Created by Peter Zipfel
///////////////////////////////////////////////////////////////////////////////
class Vehicle
{
private:
	int capacity;	//max number of vehicles permitted in vehicle list
	int howmany;	//actual number of vehicles in vehicle list
	Cadac **vehicle_ptr;
public:
	Vehicle(int number=1);	//constructor, setting capacity, allocating dynamic memory
	virtual ~Vehicle();	//destructor, de-allocating dynamic memory
	void add_vehicle(Cadac &ptr);	//adding vehicle to list
	Cadac *operator[](int position);	//[] operator returns vehicle pointer
	int size();	//returning 'howmany' vehicles are stored in vehicle list
};

#endif