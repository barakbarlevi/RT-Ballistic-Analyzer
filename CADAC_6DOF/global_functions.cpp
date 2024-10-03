///////////////////////////////////////////////////////////////////////////////
//FILE: 'global_functions.cpp'
//
//Contains the global functions for the HYPER simulation.
//
//011129 Created by Peter H Zipfel
//030415 Adopted for HYPER simulation, PZi
//091216 Added WEATHER_DECK capability, PZI
//190827 Added CSV formatted plotting option, PZi
///////////////////////////////////////////////////////////////////////////////

#include "class_hierarchy.hpp"
#include <vector>
#include <sstream>
using namespace std;

///////////////////////////////////////////////////////////////////////////////
//Acquiring simulation title and option line from the input file 'input.asc'.
//Printing of title banner to screen
//
//Parameter output: *title, *options, &nmonte, &iseed
//
//Parameter input: &nmc
//
//011128 Created by Peter H Zipfel
//020919 Added 'document_input()', PZi
//030415 Adopted for HYPER simulation, PZi
///////////////////////////////////////////////////////////////////////////////

void acquire_title_options(fstream &input,char * title,char * options,int &nmonte,int &iseed,int &nmc, std::string inputFileName)
{ 
	char read[CHARN];
	char line_clear[CHARL];
	bool title_absent=true;
	int n(0);
	
	//read until 'OPTIONS' or if not encountered within 50 lines print error message
	do
	{
		n++;
		input>>read;
		if(ispunct(read[0])) input.getline(line_clear,CHARL,'\n');
		if (!strcmp(read,"TITLE"))
		{
			input.getline(title,CHARL,'\n');
			cout<<"\n"<<title<<"   "<< __DATE__ <<" "<< __TIME__ <<"\n";
			title_absent=false;
		}
		if (!strcmp(read,"MONTE"))
		{
			input>>nmonte;
			input>>iseed;
			cout<<"MONTE Run # "<<nmc+1<< "  , Input File is: " << inputFileName << '\n';
		}
	}while((strcmp(read,"OPTIONS"))&&(n<50));
	input.getline(options,CHARL,'\n');
	if(title_absent)
	{
		strcpy(title,"*** No Title found ***");
		cout<<"\n"<<title<<"   "<< __DATE__ <<" "<< __TIME__ <<"\n";

	}
	if(n==50) {cerr<<"*** Error: OPTIONS must be before MODULES; or: MONTE does not have a seed *** \n";system("pause");exit(1);} 
}

////////////////////////////////////////////////////////////////////////////////
//Acquiring the number of modules from the input file. 
//
//Parameter output: &num, number of modules, (call-by-reference)
//					&file_prt, pointer to first module in 'input.asc' 
//
//011128 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////

void number_modules(fstream &input,int &num)
{
	char temp[CHARN];
	char line_clear[CHARL];
	num=0;
	int file_ptr=(0);

	input>>temp;
	if (!strcmp(temp,"MODULES"))
	{
		input.getline(line_clear,CHARL,'\n');
		file_ptr=int(input.tellg());
		do
		{
			input>>temp;
			input.getline(line_clear,CHARL,'\n');
			num++;
		}while(strcmp(temp,"END"));
		num=num-1;
	}
	else
		cout<<"*** 'MODULES' must follow 'OPTIONS' line without comment lines *** \n";

	//reset file pointer to first module name
	input.seekg(file_ptr);
}
///////////////////////////////////////////////////////////////////
//Acquiring the ordering of the modules from the input file 'input.asc'.
//
//Argument output: *module_list, list of module names in the sequence of 'input.asc' 
//
//011128 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////

void order_modules(fstream &input,int &num,Module *module_list)
{	
	string temp;
	char module_type[CHARL];
	char line_clear[CHARL];


	for (int i=0;i<num;i++)		
	{
		input>>temp;
		module_list[i].name=temp;

		//reading the type of module functions present
		input.getline(module_type,CHARL,'\n');

		//initializing first
		module_list[i].definition="0";
		module_list[i].initialization="0";
		module_list[i].execution="0";
		module_list[i].termination="0";

		//loading the structure data
		if(strstr(module_type,"def"))module_list[i].definition="def";
		if(strstr(module_type,"init"))module_list[i].initialization="init";
		if(strstr(module_type,"exec"))module_list[i].execution="exec";
		if(strstr(module_type,"term"))module_list[i].termination="term";
	}
	input.getline(line_clear,CHARL,'\n');
}

///////////////////////////////////////////////////////////////////////////////
//Acquiring timing parameters
//
//Parameter output: plot_step, scrn_step, int_step
//
//010330 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////
//void acquire_timing(fstream &input,double &plot_step,double &scrn_step,double &int_step,
//					double &com_step,double &traj_step)

void acquire_timing(fstream& input, double& plot_step, double& int_step)
{
	char temp[CHARN];
	char line_clear[CHARL];
	plot_step=0;
	//scrn_step=0;
	int_step=0;
	//com_step=0;
	//traj_step=0;

	input>>temp;
	if (!strcmp(temp,"TIMING"))
	{
		input.getline(line_clear,CHARL,'\n');
		do
		{
			input>>temp;
			if(!strcmp(temp,"plot_step"))input>>plot_step;
			//if(!strcmp(temp,"scrn_step"))input>>scrn_step;
			if(!strcmp(temp,"int_step"))input>>int_step;
			//if(!strcmp(temp,"com_step"))input>>com_step;
			//if(!strcmp(temp,"traj_step"))input>>traj_step;
			input.getline(line_clear,CHARL,'\n');

		}while(strcmp(temp,"END"));
	}
	else
		cout<<"*** 'TIMING' must follow 'MODULES; NO blank lines between MODULES...END' ***\n";
}

///////////////////////////////////////////////////////////////////////////////
//Acquiring the number of vehicle objects from the input file 'input.asc'
//
//Parameter output: &num_vehicle, number of vehicles, (call-by-reference)
//					&num_hyper, number of hyper hypers					
//
//010330 Created by Peter H Zipfel
//020920 Added check for illigal '=' signs and missing numerical entries, PZi
//030415 Adopted for HYPER simulation, PZi
///////////////////////////////////////////////////////////////////////////////

void number_objects(fstream &input,int &num_vehicles,int &num_hyper)
{
	char read[CHARN];
	char line_clear[CHARL];
	num_vehicles=0;
	num_hyper=0;
	int file_ptr=(0);
	char comment[3]="//";

	//reading number of total vehicle objects	
	input>>read;
	if (!strcmp(read,"VEHICLES")){
		input>>num_vehicles;		
		input.getline(line_clear,CHARL,'\n');
	}
	else
		cout<<"*** 'VEHICLES' must follow 'TIMING' ***\n";

	//searching for # of hyper objects
	//saving file pointer position
	file_ptr=int(input.tellg());

	do{
		input>>read;
		input.getline(line_clear,CHARL,'\n');
		if (!strcmp(read,"HYPER6")) num_hyper++;

	}while((num_hyper)<num_vehicles);

	//resetting file pointer position
	input.seekg(file_ptr);

	//flagging illigal '=' signs
	int icount=0;
	while(!input.eof()){
		input>>read;
		if(strstr(read,comment)||!strcmp(read,"IF")){
			input.getline(line_clear,CHARL,'\n');
		}
		else{
			if(strstr(read,"=")){
				input.getline(line_clear,CHARL,'\n');
				icount++;
			}
		}
	}	

	//resetting file pointer position
	input.clear();
	input.seekg(file_ptr);

	//flagging missing numerical entries
	int vcount=0;
	while(!input.eof()){
		input>>read;
		if(strstr(read,comment)||isupper(read[0]))
			input.getline(line_clear,CHARL,'\n');
		else{
			input>>read;
			if(strstr(read,comment)){
				input.getline(line_clear,CHARL,'\n');
				vcount++;
			}
			else
				input.getline(line_clear,CHARL,'\n');
		}
	}
	if(icount){cout<<" *** Error: "<<icount<<" illigal '=' sign(s) found  in 'input.asc' ***\n";}
	if(vcount){cout<<" *** Error: "<<vcount<<" missing numerical value(s) in 'input.asc' ***\n";}
	if(icount||vcount){system("pause");exit(1);} 

	//resetting file pointer position
	input.clear();
	input.seekg(file_ptr);
}

///////////////////////////////////////////////////////////////////////////////
//Allocating dynamic memory for an object by defining the appropriate pointer
//  
//Parameter output: *obj, type-of-vehicle pointer
//Arguments of object: module_list, num_modules to be passed 
// to the constructor of 'Hyper'
//Return output: type, type-of-vehicle as defined in 'input.asc'
//
//011128 Created by Peter H Zipfel
//030415 Adopted for HYPER simulation, PZi
///////////////////////////////////////////////////////////////////////////////

Cadac *set_obj_type(fstream &input,Module *module_list,int num_modules)				   
{
	char line_clear[CHARL];
	char temp[CHARN];
	Cadac *obj=NULL;
	int file_ptr=(0);
	(void)file_ptr; // xxxx

	//diagnostic: file pointer
	file_ptr=int(input.tellg());

	//bypassing comment lines
	do{
		input>>temp;
		if(ispunct(temp[0]))
			input.getline(line_clear,CHARL,'\n');
	}while(ispunct(temp[0]));

	if (!strcmp(temp,"HYPER6"))
	{
		//the pointer 'obj' is allocated the type 'Hyper' 
		obj=new Hyper(module_list,num_modules); 
		if(obj==0){cerr<<"*** Error:'obj' allocation failed *** \n";system("pause");exit(1);} 
		obj->set_name("HYPER6");
	}
	return obj;
}

///////////////////////////////////////////////////////////////////////////////
//Acquiring the simulation ending time from 'input.asc'
//
//011128 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////

double acquire_endtime(fstream &input)
{	
	double num(0);
	char read[CHARN];
	char line_clear[CHARL];
	int file_ptr=(0);
	(void)file_ptr; // xxxx

	//resetting file pointer to beginning
	file_ptr=int(input.tellg()); //note: for test only
	input.seekg(ios::beg);
	file_ptr=int(input.tellg()); //note: for test only
	do
	{
		file_ptr=int(input.tellg()); //note: for test only

		input>>read;
		if(strcmp(read,"ENDTIME"))
			input.getline(line_clear,CHARL);
	}while(strcmp(read,"ENDTIME"));

	input>>num;

	//placing file pointer at the beginning for MONTE looping
	input.seekg(ios::beg);

	return num;
}


///////////////////////////////////////////////////////////////////////////////
///////////// Definition of Member functions of class 'Variable' //////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//Initialization of module-variables of type 'real'
//
// private class member output: name, rval, def, mod, role, out
//
//001128 Created by Peter Zipfel
//020909 Added error code handling, PZi
///////////////////////////////////////////////////////////////////////////////
void Variable::init(const char * na,double rv,const char * de,const char * mo,const char * ro,const char * ou)
{

	if(!strcmp(name,"empty")==0) error[0]='*';
	strcpy(name,na);
	rval=rv;
	strcpy(def,de);
	strcpy(mod,mo);
	strcpy(role,ro);
	strcpy(out,ou);
}
///////////////////////////////////////////////////////////////////////////////
//Initialization of module-variables of type 'int'
//
// privat class member output: name, type, ival, def, mod, role, out
//
//001128 Created by Peter Zipfel
//020909 Added error code handling, PZi
///////////////////////////////////////////////////////////////////////////////
void Variable::init(const char * na,const char * ty,int iv,const char * de,const char * mo,const char * ro,const char * ou)
{
	if(!strcmp(name,"empty")==0) error[0]='*';
	strcpy(name,na);
	strcpy(type,ty);
	ival=iv;
	strcpy(def,de);
	strcpy(mod,mo);
	strcpy(role,ro);
	strcpy(out,ou);
}
///////////////////////////////////////////////////////////////////////////////
//Initialization of module-variables of type 'Matrix' for 3x1 vectors
//
//private class member output: name, VEC, def, mod, role, out
//
//001128 Created by Peter Zipfel
//020909 Added error code handling, PZi
///////////////////////////////////////////////////////////////////////////////

void Variable::init(const char * na,double v1,double v2,double v3,const char * de,const char * mo,const char * ro,const char * ou)
{
	double *pbody=NULL;
	pbody=VEC.get_pbody();
	*pbody=v1;
	*(pbody+1)=v2;
	*(pbody+2)=v3;

	if(!strcmp(name,"empty")==0) error[0]='*';
	strcpy(name,na);
	strcpy(def,de);
	strcpy(mod,mo);
	strcpy(role,ro);
	strcpy(out,ou);
}

///////////////////////////////////////////////////////////////////////////////
//Initialization of module-variables of type 'Matrix' for 3x3 matrices
//
//private class member output: name, MAT, def, mod, role, out
//
//001226 Created by Peter Zipfel
//020104 Corrected element assigment errors, PZi
//020909 Added error code handling, PZi
///////////////////////////////////////////////////////////////////////////////

void Variable::init(const char * na,double v11,double v12,double v13,double v21,double v22,double v23,
					double v31,double v32,double v33,const char * de,const char * mo,const char * ro,const char * ou)
{
	double *pbody=NULL;
	pbody=MAT.get_pbody();
	*pbody=v11;
	*(pbody+1)=v12;
	*(pbody+2)=v13;
	*(pbody+3)=v21;
	*(pbody+4)=v22;
	*(pbody+5)=v23;
	*(pbody+6)=v31;
	*(pbody+7)=v32;
	*(pbody+8)=v33;

	if(!strcmp(name,"empty")==0) error[0]='*';
	strcpy(name,na);
	strcpy(def,de);
	strcpy(mod,mo);
	strcpy(role,ro);
	strcpy(out,ou);
}
