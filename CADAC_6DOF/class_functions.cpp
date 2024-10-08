///////////////////////////////////////////////////////////////////////////////
//FILE: 'class_functions.cpp'
//
// Member functions of 'Cadac' class hierarchy
// Member functions of class 'Variable'
// 
//010628 Created by Peter H Zipfel
//030415 Adopted for HYPER, PZi
///////////////////////////////////////////////////////////////////////////////

#include "class_hierarchy.hpp"
#include "global_header.hpp"

///////////////////////////////////////////////////////////////////////////////
//////////////// Member functions of 'Cadac' class hierarchy //////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//Constructor initializing   
//
//010220 Created by Peter H Zipfel
//030415 Adopted to HYPER simulation, PZi
//091214 Modified for ROCKET6, PZi
///////////////////////////////////////////////////////////////////////////////

Round6::Round6()
{
	//creating module-variable array
	round6=new Variable[NROUND6];
	if(round6==0){cerr<<"*** Error: round6[] allocation failed ***\n";system("pause");exit(1);} 

	//zeroeing module-variable array
	for(int i=0;i<NROUND6;i++)round6[i].init("empty",0," "," "," "," ");
}
///////////////////////////////////////////////////////////////////////////////
//Constructor initializing the modules and the module-variable arrays
//Reading input data from 'input.asc' and putting into 'round6' and 'hyper' arrays 
//Writing banners to screen, 'tabout.asc' and to 'traj.asc' files  
//
//Module-variable arrays:
//	hyper[NHYPER]		contains variables of modules under class 'Hyper'
//	hyper6[nhyper6]	contains variables of all modules with empty slots removed
//	scrn_hyper6[nscrn_hyper6]	contains variables to be displayed at screen and 'tabout.asc'
//	plot_hyper6[nplot_hyper6] contains variables to be plotted, i.e., written to 'traj.asc'
//	com_hyper6[ncom_hyper6] contains variables for communication among vehicles
//  event_ptr_list[NEVENT] event pointer list
//
//Index pointer arrays:	
//	round6_scrn_ind[round6_scrn_count];
//	hyper_scrn_ind[hyper_scrn_count];
//	round6_plot_ind[round6_plot_count];
//	hyper_plot_ind[hyper_plot_count];
//	round6_com_ind[round6_com_count];
//	hyper_com_ind[hyper_com_count];
//				  
//001220 Created by Peter H Zipfel
//030415 Adopted to HYPER simulation, PZi
///////////////////////////////////////////////////////////////////////////////

Hyper::Hyper(Module *module_list,int num_modules)
{
	int i(0);

	//creating module-variable array
	hyper=new Variable[NHYPER];
	if(hyper==0){cerr<<"*** Error: hyper[] allocation failed ***\n";system("pause");exit(1);} 

	//zeroeing module-variable array
	for(i=0;i<NHYPER;i++)hyper[i].init("empty",0," "," "," "," ");

	//calling initializer modules to build 'round6' and 'hyper' arrays
	//and make other initial calculations in the following sequence

	//call the module definitions -MOD: insert here new module definition function
	for (int j=0;j<num_modules;j++)
		{
			if((module_list[j].name=="newton")&&(module_list[j].definition=="def"))
				def_newton();
			else if((module_list[j].name=="kinematics")&&(module_list[j].definition=="def"))
				def_kinematics();
			else if((module_list[j].name=="euler")&&(module_list[j].definition=="def"))
				def_euler();
			else if((module_list[j].name=="environment")&&(module_list[j].definition=="def"))
				def_environment();
			else if((module_list[j].name=="aerodynamics")&&(module_list[j].definition=="def"))
				def_aerodynamics();
			else if((module_list[j].name=="forces")&&(module_list[j].definition=="def"))
				def_forces();
			else if((module_list[j].name=="propulsion")&&(module_list[j].definition=="def"))
				def_propulsion();
			else if((module_list[j].name=="intercept")&&(module_list[j].definition=="def"))
				def_intercept();
		}
	//sizing module-variable arrays 'hyper6','scrn_hyper6','plot_hyper6' arrays
	//their dimensions are the protected data:'nhyper6','nscrn_hyper6','nplot_hyper6'
	sizing_arrays();

	//allocating dynamic memory to the module-variable arrays
	hyper6=new Variable[nhyper6];		
	if(!hyper6){cerr<<"*** Error: hyper6[] allocation failed *** \n";system("pause");exit(1);} 

	plot_hyper6=new Variable[nplot_hyper6];
	if(!plot_hyper6){cerr<<"*** Error: plot_hyper6[] allocation failed *** \n";system("pause");exit(1);} 

	// allocating memory for the screen index arrays
	//round6_scrn_ind=new int[round6_scrn_count];
	//hyper_scrn_ind=new int[hyper_scrn_count];

	// allocating memory for the plot index arrays
	round6_plot_ind=new int[round6_plot_count];
	hyper_plot_ind=new int[hyper_plot_count];

	//building 'hyper6' array (compacting and merging 'round6' and 'hyper' arrays)
	vehicle_array();

	//building 'plot_hyper6' array from 'hyper6' array
	plot_array();

	//building the index arrays of the data to be written to the 'ploti.asc' files
	plot_index_arrays();

}
///////////////////////////////////////////////////////////////////////////////
//Destructor deallocating dynamic memory
//				  
//010115 Created by Peter H Zipfel
//030415 Adopted to HYPER simulation, PZi
///////////////////////////////////////////////////////////////////////////////

Hyper::~Hyper()
{
	delete [] hyper;
	delete [] hyper6;
	delete [] round6;
	//delete [] scrn_hyper6;
	delete [] plot_hyper6;
	//delete [] com_hyper6;
	//delete [] round6_scrn_ind;
	//delete [] hyper_scrn_ind;
	delete [] hyper_plot_ind;
	delete[] round6_plot_ind;
	//delete [] round6_com_ind;
	//delete [] hyper_com_ind;
	//delete [] &event_ptr_list;
}
///////////////////////////////////////////////////////////////////////////////
//////////////////// Members of class 'Vehicle' ///////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//Constructor of class 'Vehicle'
//allocating dynamic memory for the array of pointers of type 'Cadac'
//and returning the pointer to array
//
//010626 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////
Vehicle::Vehicle(int number)
{
	capacity=number;
	vehicle_ptr=new Cadac *[capacity];
	if(vehicle_ptr==0){cerr<<"*** Error:'vehicle_ptr' allocation failed *** \n";system("pause");exit(1);} 
	howmany=0;
}

///////////////////////////////////////////////////////////////////////////////
//Destructor of class 'Vehicle'
//de-allocating dynamic memory of the pointer-array of type 'Cadac'
//
//010626 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////
Vehicle::~Vehicle()
{
	delete [] vehicle_ptr;
}

///////////////////////////////////////////////////////////////////////////////
//Adding a vehicle pointer to the vehicle list
//
//010626 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////
void Vehicle::add_vehicle(Cadac &pt)
{
	if(howmany<capacity)
		vehicle_ptr[howmany++]=&pt;			
}
///////////////////////////////////////////////////////////////////////////////
//Overloadied operator [] returns a 'Cadac' pointer
//
//010626 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////
Cadac * Vehicle::operator[](int position)
{
	if(position>=0 && position<howmany)
		return vehicle_ptr[position];
	else
	{
		cout<<"*** Bad value: "<<position<<'\n';
		return 0;
	}
}

///////////////////////////////////////////////////////////////////////////////
//Optaining size of vehicle list (total number of vehicles)
//
//010626 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////
int Vehicle::size()
{
	return howmany;
}

