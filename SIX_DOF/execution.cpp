///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////   CADAC++ Simulation ROCKET6G  ////////////////////////
///////////////////////////////////////////////////////////////////////////////
//FILE: execution.cpp
//
//Initializing and executing the simulation
//
//020923 Created by Peter H Zipfel
//030415 Migrated to HYPER simulation, PZi
//050103 Converted to GSWS6 simulation, PZi
//091204 Modified for ROCKET6 simulation, PZi
//130805 Compatible with MS Visual C++ V10, PZi
//131025 Compatible with MS Visual C++ V12, PZi
//151006 Modified for Book: INS/GPS/Star-Tracker, PZi
//190827 Added CSV formatted plotting option, PZi
//230616 Minor correction applied, PZi
///////////////////////////////////////////////////////////////////////////////

#include "class_hierarchy.hpp"
#include <chrono>
#include <string_view>
#include <cstring>
#include <thread>

///////////////////////////////////////////////////////////////////////////////
//////////////// Declaration of global function prototypes used in main() //////
///////////////////////////////////////////////////////////////////////////////

//acquiring the simmulation title
void acquire_title_options(fstream &input,char * title,char * options,int &nmonte
						   ,int &iseed,int &nmc, std::string inputFileName);

//acquiring the simulation run time
double acquire_endtime(fstream &input);

//numbering the modules
void number_modules(fstream &input,int &num);

//acquiring the calling order of the modules
void order_modules(fstream &input,int &num,Module *module_list);

//acquiring the number of vehicle objects
void number_objects(fstream &input,int &num_vehicles,int &num_hyper);

//creating a type of vehicle object
Cadac *set_obj_type(fstream &input,Module *module_list,int num_modules);

//running the simulation
void execute(Vehicle& vehicle_list, Module* module_list, double sim_time,
	double end_time, int num_vehicles, int num_modules, double plot_step,
	double int_step, const char* options, ofstream* plot_ostream_list,
	const char* title, int nmonte, int nmc);

//getting timimg cycles for plotting, screen output and integration
void acquire_timing(fstream& input, double& plot_step, double& int_step);


///////////////////////////////////////////////////////////////////////////////
// ///////////////////////////////  main()   //////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//Main function
//
//Barak
//011128 Created by Peter H Zipfel
//030415 Adopted for HYPER simulation, PZi
//050103 Converted to GSWS6 simulation, PZi
//091204 Reduced to ROCKET6 simulation, PZi
///////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) 
{
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now(); 
	double sim_time(0); //simulation time, same as 'time'
	char title[CHARL]=""; //title from first line of 'input.asc'
	char options[CHARL]=""; //options from 'input.asc' for output
	Module *module_list=NULL; //list of modules and sequence as per 'input.asc'
	int num_modules(0); //number of modules
	double plot_step(0); //writing time step to 'plot.asc' and 'ploti.asc', i=1,2,3...
	double int_step(0); //integration step size (constant throughout simulation)
	int num_vehicles(0); //total number of vehicle objects
	int num_hyper(0); //number of hyper objects
	Cadac *vehicle_type=NULL; //array of vehicle object pointers
	char vehicle_name[CHARN]=""; //name of each vehicle type
	double end_time(0); //run termination time from 'input.asc'
	string *plot_file_list=NULL; //array containing file names of 'ploti.asc', i=1,2,3...
	ofstream *plot_ostream_list=NULL; //array of output streams for 'ploti.asc', i=1,2,3...
	int nmonte(0); //number of MC runs to be executed
	int nmc(0); //MC counter
	int iseed(0); //seeding srand()
	
	//double int_steps[7] = {0.0005, 0.001, 0.002, 0.003, 0.004, 0.005, 0.01};
	double int_steps[1] = {0.005};

	///////////////////////////////////////////////////////////////////////////
	/////////////// Opening of files and creation of stream objects  //////////
	///////////////////////////////////////////////////////////////////////////

	//creating an input stream object and opening 'input.asc' file
	//char inputfilestr[] = "";
	//inputfilestr = argv[1];
	//fstream input(inputfilestr);
	
	fstream input(argv[1]);
	//fstream input("input_ballistic.asc");

	if(input.fail())
	{cerr<<"*** Error: File stream 'input.asc' failed to open (check spelling) ***\n";system("pause");exit(1);} 


	///////////////////////////////////////////////////////////////////////////
	////////////////////////// Monte Carlo Loop ///////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	do
	{
		//aqcuiring title statement and option selections
		acquire_title_options(input,title,options,nmonte,iseed,nmc, argv[1]);

		//initializing random number generator
		if(!nmc) srand(iseed); 

		//acquiring number of module 
		number_modules(input,num_modules);

		//dynamic memory allocation of array 'module_list'
		module_list=new Module[num_modules]; 
		if(module_list==0){cerr<<"*** Error: module_list[] allocation failed *** \n";system("pause");exit(1);} 
				
		//acquiring calling order of module 
		order_modules(input,num_modules,module_list);
		
		//acquiring the time stepping
		acquire_timing(input, plot_step, int_step);

		//acquiring number of vehicle objects from 'input.asc'
		number_objects(input,num_vehicles,num_hyper);

		//creating the 'vehicle_list' object
		// at this point the constructor 'Vehicle' is called and memory is allocated
		Vehicle vehicle_list(num_vehicles);
		
		//allocating memory for 'ploti.asc' file streams, but do it only once
		if(!nmc) plot_ostream_list=new ofstream[num_vehicles];
		if(plot_ostream_list==0){cerr<<"*** Error: plot_ostream_list[] alloc. failed *** \n";system("pause");exit(1);} 

		//allocating memory for 'ploti.asc' files, but do it only once
		if(!nmc) plot_file_list=new string[num_vehicles];
		if(plot_file_list==0){cerr<<"*** Error: plot_file_list[] alloc. failed *** \n";system("pause");exit(1);} 

		///////////////////////////////////////////////////////////////////////
		////////////////// Initializing of each vehicle object  ///////////////
		///////////////////////////////////////////////////////////////////////

		for(int i=0;i<num_vehicles;i++)
		{
			//Loading pointers of the vehicle-object classes ('Hyper') into 
			// the i-th location of 'vehicle_list'. Vehicle type is read in from 'input.asc' file. 
			//The loading process allocates dynamic memory at the pointer location
			// as required by the vehicle object 
			//The function returns the 'vehicle_type' as specified in 'input.asc' 
			//Furthermore, it passes 'module_list', 'num_modules' to the 'Hyper'constructors
			vehicle_type=set_obj_type(input,module_list,num_modules);
 				
			//add vehicle to 'vehicle_list'
			vehicle_list.add_vehicle(*vehicle_type);

			//getting the name of the type of vehicle
			strcpy(vehicle_name,vehicle_list[i]->get_vname());

			//vehicle data and tables read from 'input.asc' 
			vehicle_list[i]->vehicle_data(input,nmonte);

			//executing initialization computations -MOD: insert here new module initialization function		
			for (int j=0;j<num_modules;j++)
			{
				if((module_list[j].name=="kinematics")&&(module_list[j].initialization=="init"))
					vehicle_list[i]->init_kinematics(sim_time,int_step);
				else if((module_list[j].name=="environment")&&(module_list[j].initialization=="init"))
					vehicle_list[i]->init_environment();
				else if((module_list[j].name=="aerodynamics")&&(module_list[j].initialization=="init"))
					vehicle_list[i]->init_aerodynamics();
				else if((module_list[j].name=="propulsion")&&(module_list[j].initialization=="init"))
					vehicle_list[i]->init_propulsion();
				else if((module_list[j].name=="newton")&&(module_list[j].initialization=="init"))
					vehicle_list[i]->init_newton();
				else if((module_list[j].name=="euler")&&(module_list[j].initialization=="init"))
					vehicle_list[i]->init_euler();
			}

			//executing 'plot' and 'stat' file initialization and documentation, but only at first run
			if(!nmc)
			{		
				//creating output stream objects in 'plot_ostream_list[i]' for every file "ploti.asc"
				//currently only the HYPER6 plot files contain any data
				if(strstr(options,"y_plot"))
				{
					if(!strcmp(vehicle_name,"HYPER6"))
					{
						char index[CHARN]; //
						string plotiasc; //plot file name
						const char *  name;

						//building names for plot files
						sprintf(index,"%i",i+1);
						//plotiasc="plot"+string(index)+".asc"; //using Standard Library string constructor
						//plotiasc="/home/barak/Source_Files/CADAC/Custom/Version7/plot"+string(index)+".asc"; //using Standard Library string constructor
						plotiasc=string(argv[2]) + "/plot"+string(index)+".asc"; //using Standard Library string constructor

						plot_file_list[i]=plotiasc;
						name=plotiasc.c_str(); //using string member function to convert to char array 

						//creating output stream list 'plot_ostream_list[i]', each will write on file 'ploti.asc'
						plot_ostream_list[i].open(name); //'name' must be 'char' type

						//writing banner on 'ploti.asc'
						// XXXX REMOVING THE BANNER/TITLE FROM THE OUTPUT FILE 
						//vehicle_list[i]->plot_banner(plot_ostream_list[i],title);
					}
				}

			}//end of initializing plot, stat files and documentation

		} //end of initialization of vehicle object loop

		//Acuire ending time (last entry on 'input.asc')
		// and put file pointer at the beginning of 'input.asc' for MC repeat runs
		end_time=acquire_endtime(input);

		///////////////////////////////////////////////////////////////////////	
		/////////////////////// Simulation Execution //////////////////////////
		///////////////////////////////////////////////////////////////////////	

		int_step = int_steps[nmc];
		
		execute(vehicle_list, module_list, sim_time,
			end_time, num_vehicles, num_modules, plot_step,
			int_step, options, plot_ostream_list, title, nmonte, nmc);

		//Deallocate dynamic memory
		delete [] module_list;
		
		// If MONTE > 0 repeat 'nmonte' times
		nmc++;
	}	// at this point the destructor of the object 'Vehicle vehicle_list' is called 
	while(nmc<nmonte); 

	

	///////////////////////////////////////////////////////////////////////////	
	///////////////////////// End of Monte Carlo Loop /////////////////////////
	///////////////////////////////////////////////////////////////////////////	
	//////////////////////////// Post-Processing //////////////////////////////
	///////////////////////////////////////////////////////////////////////////	

	//Close input file streams
	input.close();

	//Deallocate dynamic memory
	delete [] plot_ostream_list;
	delete [] plot_file_list;
	
	
	std::string command = "cat " + string(argv[2]) +"/plot1.asc | awk -v OFS=',' '{print $1,$8,$9,$4,$5,$6,$7,$10,$11,$12,$13}' > " + string(argv[2]) +"/cadacOutputVDXVDX.asc";
	int systemReturn = std::system(command.c_str());
	
	std::cout << "System() return: " << systemReturn << std::endl;


	//std::this_thread::sleep_for(std::chrono::seconds(4));

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	//std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[seconds]" << std::endl;
	std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << "[ms]" << std::endl;

	

	return 0;
}
///////////////////////////////////////////////////////////////////////////////	
////////////////////////// End of main ////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////	


///////////////////////////////////////////////////////////////////////////////
//Executing the simulation
//
//Parameters:	&vehicle_list =	vehicle array - list of vehicle objects 
//								and their respective type (hyper)
//								estabished by global function 'set_obj_type'
//				*module_list = module array - list of modules and their calling sequence
//								established by global function 'order_modules'
//				sim_time = simulation time; called 'time' in output		 
//				end_time = time to stop the simulation - read from 'input.asc' 
//								by global function 'acquire_end'
//				num_vehicles = total number of vehicles - read from 'input.asc' (VEHICLES #)
//								by global function 'number_vehicles'  				
//				num_modules = total number of modules - read from 'input.asc'
//								by global function 'number_modules'
//				plot_step = output writing interval to file 'traj.asc' - sec
//								read from 'input.asc' by global function 'acquire_timing'  				
//				int_step = integration step 
//								read from 'input.asc' by global function 'acquire_timing'  				
//				scrn_step = output writing interval to console - sec  				
//								read from 'input.asc' by global function 'acquire_timing'
//				com_step = output interval to communication bus 'combus'
//				traj_step = output interval to 'traj.asc'
//				*options = output option list
//				&ftabout = output file-stream to 'tabout.asc'
//				*plot_ostream_list = output file-steam list of 'ploti.asc' for each individual hyper 
//								hyper object
//				*combus = commumication bus
//				*status = health of vehicles
//				num_hyper = number of 'Hyper' objects
//				&ftraj = output file-stream to 'traj.asc'
//				*title = idenfication of run
//				traj_merge = flag for merging MC runs in 'traj.asc'
//				nmonte = max number of MC runs
//				nmc = current MC iteration number 
//				*stat_ostream_list = output file-steam list of 'stati.asc' for each individual hyper 
//								hyper object
//				*stati_write_term = flag for writing impact data on 'stati.asc' once
//
//Barak				  				
//011128 Created by Peter H Zipfel
//030415 Adopted for HYPER simulation, PZi
//040315 Calculating event_time, PZi
///////////////////////////////////////////////////////////////////////////////
//void execute(Vehicle &vehicle_list,Module *module_list,double sim_time,
	//		 double end_time,int num_vehicles,int num_modules,double plot_step,
	//		 double int_step,double scrn_step,double com_step,double traj_step,const char * options,
	//		 ofstream &ftabout,ofstream *plot_ostream_list,int *status,
	//		 int num_hyper,ofstream &ftraj,const char * title,bool traj_merge,
	//		 int nmonte,int nmc,ofstream *stat_ostream_list,bool *stati_write_term)
void execute(Vehicle& vehicle_list, Module* module_list, double sim_time,
	double end_time, int num_vehicles, int num_modules, double plot_step,
	double int_step, const char* options, ofstream* plot_ostream_list,
	const char* title, int nmonte, int nmc)
{
	double plot_time(0);
	int vehicle_slot(0);
	bool increment_plot_time(false);
	bool plot_merge(false);
	int health(1);
	int* phealth = &health;

	//integration loop
	while ((sim_time<=(end_time+int_step)) && health==1)
	{
		//vehicle loop
		for (int i=0;i<num_vehicles;i++)
		{
			//slot occupied by current vehicle in 'vehicle_list[]'
			vehicle_slot=i;
				//module loop -MOD: insert here new module function
				for(int j=0;j<num_modules;j++)
				{
					if(module_list[j].name=="kinematics")
						//vehicle_list[i]->kinematics(sim_time,vehicle_list[i]->event_time,int_step,out_fact);
						vehicle_list[i]->kinematics(sim_time, int_step);
					else if(module_list[j].name=="newton")
						vehicle_list[i]->newton(int_step);
					else if(module_list[j].name=="euler")
						vehicle_list[i]->euler(int_step);
					else if(module_list[j].name=="environment")
						vehicle_list[i]->environment(int_step);
					else if(module_list[j].name=="aerodynamics")
						vehicle_list[i]->aerodynamics(int_step);
					else if(module_list[j].name=="forces")
						vehicle_list[i]->forces();
					else if(module_list[j].name=="propulsion")
						vehicle_list[i]->propulsion(int_step);
					else if(module_list[j].name=="intercept")
						vehicle_list[i]->intercept(num_vehicles,vehicle_slot,int_step,title,phealth);
				} //end of module loop

			if (fabs(plot_time - sim_time) < (int_step / 2 + EPS))
			{
				vehicle_list[i]->plot_data(plot_ostream_list[i], plot_merge);
				if (i == (num_vehicles - 1)) plot_time += plot_step;
			}

		} //end of vehicle loop

		//advancing time
		sim_time+=int_step;

	} //end of integration loop

	//writing last integration out to 'ploti.asc' 
	//with time set to '-1' for multiple CADAC-Studio plots
	if(strstr(options,"y_plot"))
	{
		plot_merge=true;
		for (int i=0;i<num_vehicles;i++)
			vehicle_list[i]->plot_data(plot_ostream_list[i],plot_merge);
	}

} 