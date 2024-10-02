///////////////////////////////////////////////////////////////////////////////
//FILE: 'hyper_functions.cpp'
//
// Contains utilitiy functions for the 'Hyper' class:
//		array sizing
//		writing banners to output
//		writing data to output
//
//001222 Created by Peter H Zipfel
//030415 Adapted to HYPER6 simulation, PZi
//091216 Added WEATHER_DECK, PZI
///////////////////////////////////////////////////////////////////////////////

#include "class_hierarchy.hpp"

#include <iomanip>

///////////////////////////////////////////////////////////////////////////////
//Determining dimensions of arrays: 'hyper6', 'scrn_hyper6', 'plot_hyper6'
//and 'com_hyper6'
// 
//Out to Hyper:: nhyper6, nscrn_hyper6, nplot_hyper6, ncom_hyper6,
//	round6_scrn_count, hyper_scrn_count, round6_plot_count, hyper_plot_count 		 ,
//
//001212 Created by Peter Zipfel
//030404 Adapted to HYPER6 simulation, PZi
///////////////////////////////////////////////////////////////////////////////
void Hyper::sizing_arrays()
{
	int i(0);
	const char *  key1="empty";
	const char * key2="scrn";
	const char * key3="plot";
	const char * key4="com";
	int round6_full_count=0;
	int hyper_full_count=0;

	//initialize 'Round6' and 'Hyper' member variables
	round6_plot_count=0;
	hyper_plot_count=0;

	//counting in 'round6' array
 	for(i=0;i<NROUND6;i++)
	{
		if(strcmp(round6[i].get_name(),key1))
			round6_full_count++;
		if(strstr(round6[i].get_out(),key2))
			round6_scrn_count++;
		if(strstr(round6[i].get_out(),key3))
			round6_plot_count++;
		//if(strstr(round6[i].get_out(),key4))
			//round6_com_count++;
	}
	//counting in 'hyper' array
 	for(i=0;i<NHYPER;i++)
	{
		if(strcmp(hyper[i].get_name(),key1))
			hyper_full_count++;

		if(strstr(hyper[i].get_out(),key3))
			hyper_plot_count++;

	}
	//output to Hyper::protected
	nhyper6=round6_full_count+hyper_full_count;
	nplot_hyper6=round6_plot_count+hyper_plot_count;
	//ncom_hyper6=round6_com_count+hyper_com_count;
}
///////////////////////////////////////////////////////////////////////////////
//Building 'hyper6' module-array by eliminating empty slots in 'round6' and 'hyper'
//and merging the two arrays 
//
//Output: Hyper::hyper6[] 
//
//001212 Created by Peter Zipfel
//030404 Adapted to HYPER6 simulation, PZi
///////////////////////////////////////////////////////////////////////////////
void Hyper::vehicle_array()
{
	const char *  test="empty";
	int i(0);

	//load nonempty slots from round6 array into hyper6 array
	int k=0;
	for(i=0;i<NROUND6;i++)
	{
		if(strcmp(round6[i].get_name(),test))
		{
			hyper6[k]=round6[i];
			k++;
		}
	}	
	//load nonempty slots from hyper array into hyper6 array	
	int m=0;
	for(i=0;i<NHYPER;i++)
	{
		if(strcmp(hyper[i].get_name(),test))
		{
			hyper6[k+m]=hyper[i];
			m++;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
//Building 'plot_hyper6' module-array from 'hyper6' array by keying on the word 'plot'
//
//Output: Hyper::plot_hyper6[] 
//
//001214 Created by Peter Zipfel
//030404 Adapted to HYPER6 simulation, PZi
///////////////////////////////////////////////////////////////////////////////
void Hyper::plot_array()
{
	int k=0;
	const char * buff=NULL;
	const char * key="plot";

	for(int i=0;i<nhyper6;i++)
	{
		buff=hyper6[i].get_out();
		if(strstr(buff,key))
		{
			plot_hyper6[k]=hyper6[i];
			k++;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
//Reading input data from input file 'input.asc' for each hyper vehicle object
//Assigning initial values to module-variables of 'round6' and 'hyper' arrays
//Reading aero and propulsion decks
//
//The first hyper object 'input.asc' reads until the first 'END' after
//'HYPER6'. The second hyper object reads untile the second 'END', etc until
//the data for all hyper objects are read
//
//Output:	hyper6_name ('Hyper' data member)
//			round6[] data values (Round6 data member)
//			hyper[] data values ('Hyper' data member)
//			aero-tables ('Hyper' data members)
//			prop-tables ('Hyper' data members)
//			event_ptr_list[] ('Event' data members)
//			markov_list[] ('Markov' list of variables)
//
//Limitation: real and integer variables can be read only (could be expanded to vectors)			 
//
//001230 Created by Peter Zipfel
//010924 Added reading of random variables, PZi
//020723 Included and initialized Markov 'saved' value, PZi
//050121 Corrected problem reading reused names (Error code 'A'), PZi 
///////////////////////////////////////////////////////////////////////////////
void Hyper::vehicle_data(fstream &input,int nmonte)
{
	char line_clear[CHARL];
	char read[CHARN];	//name of variable read from input.asc
	const char * buff=NULL;	//name of module-variable
	double data(0);	//data of module-variable 
	char file_name[CHARN]; //name of aero-deck file
//	char temp[CHARN]; //buffer for table data
//	char table_title[CHARN];
//	const char * aero_deck_title;
//	const char * prop_deck_title;
//	int dim;
	const char * integer=NULL;
	int int_data(0);
	char buff2[CHARN];
	int file_ptr=NULL;
	char name[CHARN];
	Variable *variable=NULL;
	double value(0);
	char oper;
	int e=0;
	const char * watchpoint=NULL;
	double first(0);
	double second(0);
	char name1[CHARN];
	int count1(0);
	int i(0);
	int ii(0);
	int m(0);
	int k(0);
	int kk(0);

	input.getline(hyper6_name,CHARL,'\n');

	//reading data until 'END' is encountered
	do
	{
		//reading variable data into module-variable arrays after discarding comments
		input>>read;
		if(ispunct(read[0]))
		{
			input.getline(line_clear,CHARL,'\n');
		}
		else
		{
			int count=0;
			for(i=0;i<NROUND6;i++)
			{
				buff=round6[i].get_name();
				if(!strcmp(buff,read)&&!count) 
				{
					input>>data;
					//checking for integers
					integer=round6[i].get_type();
					if(!strcmp(integer,"int"))
					{
						//loading integer value
						int_data=(int)data;
						round6[i].gets(int_data);
						input.getline(line_clear,CHARL,'\n');
					}
					else
					{
						//loading real value
						round6[i].gets(data);
						input.getline(line_clear,CHARL,'\n');
					}
					count++;
				}				
			}
			count=0;						
			for(i=0;i<NHYPER;i++)
			{
				buff=hyper[i].get_name();
				if(!strcmp(buff,read)&&!count)	
				{
					input>>data;
					//checking for integers
					integer=hyper[i].get_type();
					if(!strcmp(integer,"int"))
					{
						//loading interger value
						int_data=(int)data;
						hyper[i].gets(int_data);
						input.getline(line_clear,CHARL,'\n');
					}
					else
					{
						//loading real value
						hyper[i].gets(data);
						input.getline(line_clear,CHARL,'\n');
					}
					count++;				
				}				
			}
			//reading aero data from aero-deck file
			if(!strcmp(read,"AERO_DECK")){
				//reading aerodeck file name
				input>>file_name;
				input.getline(line_clear,CHARL,'\n');

				read_tables(file_name,aerotable);
			}
			//reading prop data from prop-deck file
			if(!strcmp(read,"PROP_DECK")){
				//reading propdeck file name
				input>>file_name;
				input.getline(line_clear,CHARL,'\n');

				read_tables(file_name,proptable);
			}
			//reading weather data from weather-deck file
			if(!strcmp(read,"WEATHER_DECK")){
				//reading weather-deck file name
				
				//std::cout << "(Barak xxxx delete) input: " << input.rdbuf() << std::endl;
				input>>file_name;
				//std::cout << "(Barak xxxx delete) input: " << input.rdbuf() << std::endl;
				input.getline(line_clear,CHARL,'\n');

				//std::cout << "(Barak xxxx delete) file name: " << file_name << std::endl;
				//std::cout << "(Barak xxxx delete) input: " << input.rdbuf() << std::endl;
				//printf("(Barak xxxx delete) file_name: %s\n", file_name);
				read_tables(file_name,weathertable);
			}

			//loading values for random variables and building 'markov_list'

			//uniform distribution
			if(!strcmp(read,"UNI"))
			{
				input>>name1;
				input>>first;
				input>>second;
				if(!nmonte)
					value=(second-first)/2.;
				else
					value=uniform(first,second);

				//loading radom value into module-variable
				for(kk=0;kk<NROUND6;kk++)
				{
					buff=round6[kk].get_name();
					if(!strcmp(buff,name1)) 
					{
						round6[kk].gets(value);
						input.getline(line_clear,CHARL,'\n');
					}				
				}
				for(kk=0;kk<NHYPER;kk++)
				{
					buff=hyper[kk].get_name();
					if(!strcmp(buff,name1)) 
					{
						hyper[kk].gets(value);
						input.getline(line_clear,CHARL,'\n');
					}				
				}
			}

			//Gaussian distribution
			if(!strcmp(read,"GAUSS"))
			{
				input>>name1;
				input>>first;
				input>>second;
				if(!nmonte)
					value=first;
				else
					value=gauss(first,second);

				//loading radom value into module-variable
				for(kk=0;kk<NROUND6;kk++)
				{
					buff=round6[kk].get_name();
					if(!strcmp(buff,name1)) 
					{
						round6[kk].gets(value);
						input.getline(line_clear,CHARL,'\n');
					}				
				}
				for(kk=0;kk<NHYPER;kk++)
				{
					buff=hyper[kk].get_name();
					if(!strcmp(buff,name1)) 
					{
						hyper[kk].gets(value);
						input.getline(line_clear,CHARL,'\n');
					}				
				}
			}

			//Rayleigh distribution
			if(!strcmp(read,"RAYL"))
			{
				input>>name1;
				input>>first;
				if(!nmonte)
					value=first;
				else
					value=rayleigh(first);

				//loading radom value into module-variable
				for(kk=0;kk<NROUND6;kk++)
				{
					buff=round6[kk].get_name();
					if(!strcmp(buff,name1)) 
					{
						round6[kk].gets(value);
						input.getline(line_clear,CHARL,'\n');
					}				
				}
				for(kk=0;kk<NHYPER;kk++)
				{
					buff=hyper[kk].get_name();
					if(!strcmp(buff,name1)) 
					{
						hyper[kk].gets(value);
						input.getline(line_clear,CHARL,'\n');
					}				
				}
			}

			//exponential distribution
			if(!strcmp(read,"EXP"))
			{
				input>>name1;
				input>>first;
				if(!nmonte)
					value=first;
				else
					value=exponential(first);

				//loading radom value into module-variable
				for(kk=0;kk<NROUND6;kk++)
				{
					buff=round6[kk].get_name();
					if(!strcmp(buff,name1)) 
					{
						round6[kk].gets(value);
						input.getline(line_clear,CHARL,'\n');
					}				
				}
				for(kk=0;kk<NHYPER;kk++)
				{
					buff=hyper[kk].get_name();
					if(!strcmp(buff,name1)) 
					{
						hyper[kk].gets(value);
						input.getline(line_clear,CHARL,'\n');
					}				
				}
			}

		} //end of reading non-comment lines
		//stopping if misspellings in 'input.asc'
	}while(strcmp(read,"END")); //reached 'END' of vehicle object

	//flushing the line after END and starting new line
	input.getline(line_clear,CHARL,'\n'); 

	//diagnostic: file pointer
	file_ptr=int(input.tellg());
}

///////////////////////////////////////////////////////////////////////////////
//Writing out banner of labels to 'ploti.asc', i=1,2,3...
//
//First label is time-since-launch-of-vehicle 'time', always at round6[0]
//five accross, unlimited down
//data field width 16 spaces, total width 80 spaces
//labels longer than 8 characters will be truncated
//Accomodates 3x1 vectors
//
//010115 Created by Peter Zipfel
//030404 Adapted to HYPER6 simulation, PZi
///////////////////////////////////////////////////////////////////////////////
void Hyper::plot_banner(ofstream &fplot,const char * title)
{
	const char * buff1=NULL;
	char buff2[15];
	int label_length=13;
	int i(0);
	int k(0);
	int m(0);

	//fplot<<"1"<<title<<" '"<<hyper6_name<<" ' "<< __DATE__ <<" "<< __TIME__ <<"\n";
	
	//determining the number vector variables
	for(i=0;i<nplot_hyper6;i++)
	{
		buff1=plot_hyper6[i].get_name();
		if(isupper(buff1[0])) m++;
	}
	//increase number of variables by vector components
	int nvariables=nplot_hyper6+2*m;
	
	//fplot<<"  0  0 " <<nvariables<<"\n"; 

	//writing banner to plot file 'ploti.asc'
	for(i=0;i<nplot_hyper6;i++)
	{
		fplot.setf(ios::left);

		buff1=plot_hyper6[i].get_name();

		//truncating if more than 8 characters
		strncpy(buff2,buff1,label_length);
		buff2[13]=0;

		//Vectors are recognized by upper case character 
		if(isupper(buff2[0]))
		{
			for(int i=1;i<4;i++)
			{				
				fplot.width(strlen(buff2));
				fplot<<buff2;fplot.width(16-strlen(buff2));fplot<<i;
				k++;
				if(k>4){
					k=0;
					//fplot<<'\n';	// REMOVING \ns
					}
			}
		}
		else
		{
			fplot.width(16);
			fplot<<buff2;
			k++;
			if(k>4){
				k=0;
				//fplot<<'\n';	// REMOVING \ns
				}
		}


	}
	
	if((nvariables%10))fplot<<"\n";	// REMOVING \ns
}
///////////////////////////////////////////////////////////////////////////////
//Building index array of those 'round6[]' and hyper[] variables that are  
//output to 'ploti.asc'
//
//Output: Hyper::round6_plot_ind[], hyper_plot_ind[] 
//
//001213 Created by Peter Zipfel
//030404 Adapted to HYPER6 simulation, PZi
///////////////////////////////////////////////////////////////////////////////
void Hyper::plot_index_arrays()
{
	const char *  test="plot";
	int i(0);
	int k(0);
	for(i=0;i<NROUND6;i++)
	{
		if(strstr(round6[i].get_out(),test))
		{
			round6_plot_ind[k]=i;
			k++;
		}
	}
	int l=0;
	for(i=0;i<NHYPER;i++)
	{
		if(strstr(hyper[i].get_out(),test))
		{
			hyper_plot_ind[l]=i;
			l++;
		}
	}
}
///////////////////////////////////////////////////////////////////////////////
//Writing data to 'ploti.asc', i=1,2,3...
//
//Accomodates real, integers (printed as real) and 3x1 vectors 
//five accross, unlimited down
//data field 16 spaces, total width 80 spaces
//
//010116 Created by Peter Zipfel
//030404 Adapted to HYPER6 simulation, PZi
///////////////////////////////////////////////////////////////////////////////
void Hyper::plot_data(ofstream &fplot,bool merge)
{

	int index(0);
	const char * integer=NULL;
	const char * vector=NULL;
	Matrix VEC(3,1);
	int k(0);
	int i(0);
	
	fplot.setf(ios::left);

	//writing to 'ploti.asc' the variables from the 'Round6' class
	for(i=0;i<round6_plot_count;i++)
	{
		index=round6_plot_ind[i];
		//checking for integers
		integer=round6[index].get_type();
		vector=round6[index].get_name();
		if(!strcmp(integer,"int"))
		{
			//casting integer to real variable
			if(k>4){
				k=0;
				//fplot<<'\n';	// REMOVING \ns
				}
			fplot.width(16);
			fplot<<(double) round6[index].integer();	// Original			
			//fplot << std::setprecision(4) << fixed << (double) round6[index].integer();
			k++;
		}
		//checking vor vectors
		else if(isupper(vector[0]))
		{
			VEC=round6[index].vec();

			if(k>4){
				k=0;
				//fplot<<'\n';	// REMOVING \ns
				}
			fplot.width(16);
			fplot<<VEC.get_loc(0,0);	// Original
			//fplot << std::setprecision(4) << fixed << VEC.get_loc(0,0);
			k++; 
			if(k>4){
				k=0;
				//fplot<<'\n';	// REMOVING \ns
				}
			fplot.width(16);
			fplot<<VEC.get_loc(1,0);	// Original
			//fplot << std::setprecision(4) << fixed << VEC.get_loc(1,0);
			k++;
			if(k>4){
				k=0;
				//fplot<<'\n';	// REMOVING \ns
				}
			fplot.width(16);
			fplot<<VEC.get_loc(2,0);	// Original
			//fplot << std::setprecision(4) << fixed << VEC.get_loc(2,0);
			k++;
		}
		else
		{
			//real variables
			if(merge&&(index==0))
				//for merging files, time at last entry must be '-1'
			{
				fplot.width(16);
				fplot<<"-1.0";
				k++;
			}
			else
			{
				if(k>4){
					k=0;
					//fplot<<'\n';	// REMOVING \ns
					}
				fplot.width(16);
				fplot<<round6[index].real();	// Original 
				//fplot << std::setprecision(4) << fixed << round6[index].real();
				k++;
			}
		}
	}
	//writing to 'ploti.asc' the variables from the 'Hyper' class
	for(i=0;i<hyper_plot_count;i++)
	{
		index=hyper_plot_ind[i];
		//checking for integers
		integer=hyper[index].get_type();
		vector=hyper[index].get_name();
		if(!strcmp(integer,"int"))
		{
			//casting integer to real variable
			if(k>4){
				k=0;
				//fplot<<'\n';	// REMOVING \ns
				}
			fplot.width(16);
			fplot<<(double) hyper[index].integer();	// Original	
			//fplot << std::setprecision(4) << fixed << (double) hyper[index].integer();
			k++;

		}
		//checking vor vectors
		else if(isupper(vector[0]))
		{
			VEC=hyper[index].vec();
			if(k>4){
				k=0;
				//fplot<<'\n';	// REMOVING \ns
				}
			fplot.width(16);
			fplot<<VEC.get_loc(0,0);	// Original
			//fplot << std::setprecision(4) << fixed << VEC.get_loc(0,0);
			k++;
			if(k>4){
				k=0;
				//fplot<<'\n';	// REMOVING \ns
				}
			fplot.width(16);
			fplot<<VEC.get_loc(1,0);	// Original
			//fplot << std::setprecision(4) << fixed << VEC.get_loc(1,0);
			k++;
			if(k>4){
				k=0;
				//fplot<<'\n';	// REMOVING \ns
				}
			fplot.width(16);
			fplot<<VEC.get_loc(2,0);	// Original
			//fplot << std::setprecision(4) << fixed << VEC.get_loc(2,0);
			k++;
		}
		else
		{
			//real variables
			if(k>4){
				k=0;
				//fplot<<'\n';	// REMOVING \ns
				}
			fplot.width(16);
			fplot<<hyper[index].real();	// Original
			//fplot << std::setprecision(4) << fixed << hyper[index].real();
			k++;
		}
	}
	fplot<<"\n";
}

///////////////////////////////////////////////////////////////////////////////
//Reading tables from table decks
//
//Supports 1, 2, 3 dim tables stored seperately in data files
// Keying on AERO_DECK, PROP_DECK and WEATHER_DECK in 'input.asc' this function  reads the tables
// from the data files and stores them in 'Datadeck aerotable' and 
// 'Datadeck proptable'. In the modules, table look up is carried out by 
// double value=aerodeck.look_up(string name, double var1);  
// double value=aerodeck.look_up(string name, double var1,double var2);  
// double value=aerodeck.look_up(string name, double var1,double var2,double var3);  
//
// To add new tables, just include them in the files of AERO_DECK, PROP_DECK and WEATHER_DECK
// For debugging puposes un-comment the print out provision of the tables below
//
//030721 Created by Peter H Zipfel
//031104 Corrected table diagnostic, PZi
//060426 Corrected line 1722 (replaced 'if else' by 'if'), PZi
///////////////////////////////////////////////////////////////////////////////
void Hyper::read_tables(const char * file_name,Datadeck &datatable)
{

	char line_clear[CHARL];
	char temp[CHARN];	//buffer for table data
	string table_deck_title;
	int table_count(0);
	int table_dim(0);
	double value(0);
	int file_ptr=NULL;
	int var_dim[3]={1,1,1,};
	int tt(0);

	//opening aero-deck file stream
	ifstream tbl_stream(file_name);
	

	if(tbl_stream.fail())
		{
			std::cout << "Encountered error. file_name = " << file_name << std::endl;
			cerr<<"*** Error: File stream '"<<file_name<<"' failed to open (check spelling) ***\n";system("pause");exit(1);
			} 

	//determing the total # of tbl_stream
	while(!tbl_stream.eof())
	{
		tbl_stream>>temp;
		if(!strcmp(temp,"TITLE")){
			tbl_stream.getline(line_clear,CHARL,'\n');
			table_deck_title=line_clear;
		}
		if(strstr(temp,"DIM"))
			table_count++;

	} //EOF reached of aero_deck

	//removing EOF bit (-1)
	tbl_stream.clear();
	//rewinding to beginning
	tbl_stream.seekg(ios::beg);

	//creating pointer array table 
	datatable.set_title(table_deck_title);
	datatable.set_capacity(table_count);
	datatable.alloc_mem();

	//discarding all entries until first DIM
	do{
		tbl_stream>>temp;
		//std::cout << "temp after: tbl_stream>>temp; in a loop for discarding all entries until first DIM: " << temp << std::endl;	// XXXX
	}while(!strstr(temp,"DIM"));

	//std::cout << "right after exiting the loop temp is: " << temp << std::endl;	// XXXX
	//std::cout << "Supposedly reached DIM" << std::endl;	// XXXX

	//loading tables one at a time
	for(int t=0;t<table_count;t++){

		//creating and allocating memory to object 'table'
		table=new Table;
			
		//extracting table dimension
		//at this point 'temp' is holding xDIM
		//std::cout << "He says at this point 'temp' is holding xDIM. temp is: " << temp << std::endl;	// XXXX

		int dim_check(0);
		char dim_buff[2]{};
		int table_dim(0);

		// xxxx
		// std::cout << "temp's address in memory is: " << &temp << std::endl;	// XXXX
		// std::cout << "dim_check (right after instantiation): " << dim_check << std::endl;
		// std::cout << "dim_buff (right after instantiation): " << dim_buff << std::endl;
		// std::cout << "dim_buff[0] (right after instantiation): " << dim_buff[0] << std::endl;
		// std::cout << "dim_buff[1] (right after instantiation): " << dim_buff[1] << std::endl;
		// std::cout << "dim_buff[2] (right after instantiation): " << dim_buff[2] << std::endl;
		// std::cout << "dim_buff[3] (right after instantiation): " << dim_buff[3] << std::endl;
		// std::cout << "dim_buff[4] (right after instantiation): " << dim_buff[4] << std::endl;
		// std::cout << "dim_buff[5] (right after instantiation): " << dim_buff[5] << std::endl;
		// std::cout << "dim_buff[6] (right after instantiation): " << dim_buff[6] << std::endl;
		// std::cout << "dim_buff (right after instantiation): " << (void *)dim_buff << std::endl;
		// std::cout << "dim_buff[0] (right after instantiation): " << (void *)dim_buff[0] << std::endl;
		// std::cout << "dim_buff[1] (right after instantiation): " << (void *)dim_buff[1] << std::endl;
		// std::cout << "dim_buff[2] (right after instantiation): " << (void *)dim_buff[2] << std::endl;
		// std::cout << "dim_buff[3] (right after instantiation): " << (void *)dim_buff[3] << std::endl;
		// std::cout << "dim_buff[4] (right after instantiation): " << (void *)dim_buff[4] << std::endl;
		// std::cout << "dim_buff[5] (right after instantiation): " << (void *)dim_buff[5] << std::endl;
		// std::cout << "dim_buff[6] (right after instantiation): " << (void *)dim_buff[6] << std::endl;
		// std::cout << "dim_buff (right after instantiation): " << &dim_buff << std::endl;
		// std::cout << "dim_buff[0] (right after instantiation): " << &dim_buff[0] << std::endl;
		// std::cout << "dim_buff[1] (right after instantiation): " << &dim_buff[1] << std::endl;
		// std::cout << "dim_buff[2] (right after instantiation): " << &dim_buff[2] << std::endl;
		// std::cout << "dim_buff[3] (right after instantiation): " << &dim_buff[3] << std::endl;
		// std::cout << "dim_buff[4] (right after instantiation): " << &dim_buff[4] << std::endl;
		// std::cout << "dim_buff[5] (right after instantiation): " << &dim_buff[5] << std::endl;
		// std::cout << "dim_buff[6] (right after instantiation): " << &dim_buff[6] << std::endl;
		// std::cout << "table_dim (right after instantiation): " << table_dim << std::endl;

		strncpy(dim_buff,temp,1);

		// xxxx
		// std::cout << "dim_buff (right after strncpy(dim_buff,temp,1);): " << dim_buff << std::endl;
		// std::cout << "dim_buff[0] (right after strncpy(dim_buff,temp,1);): " << dim_buff[0] << std::endl;
		// std::cout << "dim_buff[1] (right after strncpy(dim_buff,temp,1);): " << dim_buff[1] << std::endl;
		// std::cout << "dim_buff[2] (right after strncpy(dim_buff,temp,1);): " << dim_buff[2] << std::endl;
		// std::cout << "dim_buff[3] (right after strncpy(dim_buff,temp,1);): " << dim_buff[3] << std::endl;
		// std::cout << "dim_buff[4] (right after strncpy(dim_buff,temp,1);): " << dim_buff[4] << std::endl;
		// std::cout << "dim_buff[5] (right after strncpy(dim_buff,temp,1);): " << dim_buff[5] << std::endl;
		// std::cout << "dim_buff[6] (right after strncpy(dim_buff,temp,1);): " << dim_buff[6] << std::endl;
		// std::cout << "dim_buff (right after strncpy(dim_buff,temp,1);): " << (void *)dim_buff << std::endl;
		// std::cout << "dim_buff[0] (right after strncpy(dim_buff,temp,1);): " << (void *)dim_buff[0] << std::endl;
		// std::cout << "dim_buff[1] (right after strncpy(dim_buff,temp,1);): " << (void *)dim_buff[1] << std::endl;
		// std::cout << "dim_buff[2] (right after strncpy(dim_buff,temp,1);): " << (void *)dim_buff[2] << std::endl;
		// std::cout << "dim_buff[3] (right after strncpy(dim_buff,temp,1);): " << (void *)dim_buff[3] << std::endl;
		// std::cout << "dim_buff[4] (right after strncpy(dim_buff,temp,1);): " << (void *)dim_buff[4] << std::endl;
		// std::cout << "dim_buff[5] (right after strncpy(dim_buff,temp,1);): " << (void *)dim_buff[5] << std::endl;
		// std::cout << "dim_buff[6] (right after strncpy(dim_buff,temp,1);): " << (void *)dim_buff[6] << std::endl;
		// std::cout << "dim_buff (right after strncpy(dim_buff,temp,1);): " << &dim_buff << std::endl;
		// std::cout << "dim_buff[0] (right after strncpy(dim_buff,temp,1);): " << &dim_buff[0] << std::endl;
		// std::cout << "dim_buff[1] (right after strncpy(dim_buff,temp,1);): " << &dim_buff[1] << std::endl;
		// std::cout << "dim_buff[2] (right after strncpy(dim_buff,temp,1);): " << &dim_buff[2] << std::endl;
		// std::cout << "dim_buff[3] (right after strncpy(dim_buff,temp,1);): " << &dim_buff[3] << std::endl;
		// std::cout << "dim_buff[4] (right after strncpy(dim_buff,temp,1);): " << &dim_buff[4] << std::endl;
		// std::cout << "dim_buff[5] (right after strncpy(dim_buff,temp,1);): " << &dim_buff[5] << std::endl;
		// std::cout << "dim_buff[6] (right after strncpy(dim_buff,temp,1);): " << &dim_buff[6] << std::endl;
		
		//converting character to integer
		dim_check=sscanf(dim_buff,"%d",&table_dim);

		// xxxx
		// std::cout << "table_dim (dim_check=sscanf(...);): " << table_dim << std::endl;
		// std::cout << "dim_check (dim_check=sscanf(...);): " << dim_check << std::endl;

		table->set_dim(table_dim);
								
		//extracting table name
		//std::cout << "He says extracting table name. tbl_stream is: " << tbl_stream.rdbuf() << std::endl;	// XXXX
		tbl_stream>>temp;
		//std::cout << "setting table name with: " << temp << std::endl; // xxxx
		table->set_name(temp);
		tbl_stream.getline(line_clear,CHARL,'\n');

		//extracting dimensions of independent variables
		var_dim[0]=1;var_dim[1]=1;var_dim[2]=1;
		//std::cout << "table_dim is: " << table_dim << std::endl; // xxxx
		for(tt=0;tt<table_dim;tt++){
			tbl_stream>>temp;
			//std::cout << "temp at extracting dimensions of independent variables: " << temp << std::endl; // xxxx
			tbl_stream>>var_dim[tt];
			if(tt==0)
				table->set_var1_dim(var_dim[tt]);
			if(tt==1)
				table->set_var2_dim(var_dim[tt]);
			if(tt==2)
				table->set_var3_dim(var_dim[tt]);
		}
		tbl_stream.getline(line_clear,CHARL,'\n');

		//allocating memory for variables and data arrays
		table->var1_values=new double [var_dim[0]];
		table->var2_values=new double [var_dim[1]];
		table->var3_values=new double [var_dim[2]];
		table->data=new double[var_dim[0]*var_dim[1]*var_dim[2]];

		// xxxx
		// std::cout << "var1,2,3_values allocated here with new double." << std::endl;
		// printf("var1_values address: %p\n", (void*) &table->var1_values);
		// printf("var2_values address: %p\n", (void*) &table->var2_values);
		// printf("var3_values address: %p\n", (void*) &table->var3_values);
		// printf("table->data address: %p\n", (void*) &table->data);

		//determining max number of rows of data
		int num_rows=var_dim[0];
		if(var_dim[0]<var_dim[1]) num_rows=var_dim[1];
		if(var_dim[2]>num_rows) num_rows=var_dim[2];

		//reading num_row of data 
		for(tt=0;tt<num_rows;tt++){

			//loading 1.variable values
			if(tt<var_dim[0]){
				tbl_stream>>value;
				table->set_var1_value(tt,value);
			}

			//loading 2.variable values, but bypass if default dimension one
			if(tt<var_dim[1]&&var_dim[1]!=1){
				tbl_stream>>value;
				table->set_var2_value(tt,value);
			}

			//loading 3.variable values, but bypass if default dimension one
			if(tt<var_dim[2]&&var_dim[2]!=1){
				tbl_stream>>value;
				table->set_var3_value(tt,value);
			}

			//loading tabular data, which in all cases has only 'var_dim[0]' rows
			if(tt<var_dim[0]){

				//read one row of data
				for(int ttt=0;ttt<var_dim[1]*var_dim[2];ttt++){
					tbl_stream>>value;
					table->set_data(tt*var_dim[1]*var_dim[2]+ttt,value);
				}
			}
		}//end of reading data

		//loading table into 'Datadeck' pointer array 'Table **tabel_ptr'
		datatable.set_counter(t);
		datatable.add_table(*table);
		tbl_stream>>temp; //reading next DIM entry
		//std::cout << "He says reading next DIM entry (end of loop). temp is: " << temp << std::endl;	// XXXX
		
	}//end of 'for' loop, finished loading all tables
	
}