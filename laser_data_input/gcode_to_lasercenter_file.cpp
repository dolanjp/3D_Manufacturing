#include<iostream>
#include<string>
#include<sstream>
#include<fstream>
#include<vector>
#include<cmath>
#include<iomanip>
#include<sstream>
#include<stdlib.h>

//Command line syntax: ./gcode_to_lasercenter <input_file> <output_file>


using namespace std;

//** Global Variables **

// Define Laser Center file time step (in seconds). This does not impact the time step
// in the AMP simulation, it only determines the resolution of the Laser Center file interpolation
double time_step;

//Define data containers and counters
std::string gcode_data_labels[] = {"F","X","Y","S"};
std::vector<double> gcode_data[5]; //{F(laser scan speed), X(coordinate), Y(coordinate), S(laser power, adapted from spindle speed), Laser on or off (G0 or G1)};
unsigned int line_count = 0;

//** Function Prototypes ** 
void parse_vals (const string&);
double gcl_time (int);
void interp_write(char*);


int main(int argc, char* argv[])
{
  std::string line, line_trunc, input_filename;
  if ((argv[1] == NULL) || (argv[2] == NULL) || (argv[3] == NULL)){
	  std::cout << "Error: input and output file paths not specified properly \n";
	  return 0;	  
  }
   
  ifstream input(argv[1]);  
  if (!input){
	  std::cout << "Error: input file is not valid \n";
	  return 0;
  }
  
  time_step = atof(argv[3]); 
  std::cout << "Time step = " << time_step << std::endl;
  
  std::cout << "Started... \n";
  int temp_count = 0;
  while (getline(input,line))
  {
	if (line.find(";") != 0){  //If the entire line is not commented
	  if (line.find(";") != string::npos){ //If the end of the line is commented remove the comment
		  line_trunc = line.substr(0,line.find(";"));
	  }
  	  else{
	      line_trunc = line;
	  }
	  //If the gcode line defines a rapid or controlled linear motion
      if ((line_trunc.find("G1 ") != string::npos) && (line_trunc.find("G1 ") == 0) 
		  || (line_trunc.find("G0 ") != string::npos) && (line_trunc.find("G0 ") == 0)){
        parse_vals(line_trunc);
      }
	  //Check if other un-supported gcode commands are used
	  else if ((line_trunc.find("G2 ") != string::npos) 
		  || (line_trunc.find("G3 ") != string::npos) 
		  || (line_trunc.find("Z") != string::npos)){
		  cout << "Error: unsupported g-code commands. This interpolater only works with 2D linear gcode files";
		  return 0;
	  }
    }
  }
  //After all data is read in from the gcode file, write the Laser Center output file
  interp_write(argv[2]);

return 0;
}


void parse_vals (const string& line)
{
  double param_value;
  
  for (int i = 0; i <= 3; i++){  //Look at f,x,y,s parameters and see if they are included on the current gcode line
    string::size_type pos;
    if ((line.find(gcode_data_labels[i]) != string::npos)){  	//If the parameter is specified on the current line, add that value to the gcode data list
        pos = line.find(gcode_data_labels[i]);
        stringstream(line.substr(pos+1,(line.find(' ',pos+1)-pos))) >> param_value;  
        gcode_data[i].push_back(param_value);
    }
	else if (line_count > 0){  //If a parameter is not specified on the current gcode line, propagate the previous value of that parameter
      gcode_data[i].push_back(gcode_data[i][line_count-1]);
    }
    else{ //if (line_count == 0)  //If a parameter is not specified on the first gcode line, set it to zero by default
        gcode_data[i].push_back(0);
	}
  }
  
  
  if (line.find("G1") != string::npos){  //If G1 Controlled Movement, laser is turned on
	gcode_data[4].push_back(1); 
  }
  else{ //if (line.find("G0") != string::npos){  //If G0 Rapid Movement, laser is turned off
	gcode_data[4].push_back(0); 
  }
 
  
  //cout << "F: " << gcode_data[0][line_count] << " X: " << gcode_data[1][line_count] << "  Y:  " << gcode_data[2][line_count] << "  S:  " << gcode_data[3][line_count] << "  Line_Count:  " << line_count << "\n";
  line_count++;
}  


double gcl_time (int gcl)
{
  //determines the total time taken to move from one gcode x,y position to the next x,y position based on the current feedrate value
  return (sqrt( pow((gcode_data[1][gcl-1] - gcode_data[1][gcl]),2) + pow((gcode_data[2][gcl-1]-gcode_data[2][gcl]),2))/gcode_data[0][gcl]);
}


void interp_write(char* output_filepath)
{
  double gcode_line_remainder = 1;
  double T = 0, X = gcode_data[1][0], Y = gcode_data[2][0], S = gcode_data[3][0], Laser_On_Off = gcode_data[4][0] ;  //Initialize time and other parameters
  double time_remaining = time_step;  //initialize time counter
  int gcl = 1;  //gcode line counter
  
  //Warning: The Laser Power (Parameter "S") is hard-coded to 1 until the power_fraction is changed in the AMP and 3Dmfg code.
  // After the change, the laser power will be specified in the laser center file (in watts) instead of the materials file
  int hard_coded_laser_power = 1;
  
  ofstream outfile;
  outfile.open( output_filepath, ofstream::out | ofstream::trunc);
  
  //Write the first position at time = 0
  S = hard_coded_laser_power; //remove after specified Albany code change is made
  outfile << fixed << setprecision(10)  << T << "\t" << X << "\t" << Y << "\t" << Laser_On_Off << "\t" << S << "\n";  //Note: the order and content of these parameters may change

  while (gcl < line_count) //Note: line_count is incremented one past the end of the gcode_data list
    {  
	  //std::cout << gcl;
      if (gcode_line_remainder > time_remaining/gcl_time(gcl))
      { 
        gcode_line_remainder = gcode_line_remainder - (time_remaining/gcl_time(gcl));
        X = gcode_line_remainder*gcode_data[1][gcl-1] + (1-gcode_line_remainder)*gcode_data[1][gcl];
        Y = gcode_line_remainder*gcode_data[2][gcl-1] + (1-gcode_line_remainder)*gcode_data[2][gcl];
        time_remaining = time_step;
        T = T + time_step;
        
	  S = hard_coded_laser_power; //remove after specified Albany code change is made
      outfile << fixed << setprecision(10)  << T << "\t" << X << "\t" << Y << "\t" << Laser_On_Off << "\t" << S << "\n";  //Note: the order and content of these parameters may change
      }
	  
      else if (gcode_line_remainder < time_remaining/gcl_time(gcl))
      {
        time_remaining = time_remaining - gcl_time(gcl)*gcode_line_remainder;
        gcl++;
        gcode_line_remainder = 1; 
      }
         
    }  
outfile.close();
}
