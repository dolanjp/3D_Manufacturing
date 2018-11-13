#include "pugixml.hpp"
#include<iostream>
#include<vector>
#include <sstream>
#include <string>
#include <algorithm> 
#include <fstream>

class gcode_document{
	public:
		std::vector<int> velocity_profiles; //may need to change to floats
		double work_offset[2];
		std::vector<double> gcode_data[4];		
	
	public:
		void add_velocity_profile(int velocity);
		void add_gcode_dataset(double X, double Y, double F, double S);
		void write_gcode_file();	
		void modify_work_offset(double x, double y);
		void set_work_offset(double x, double y);
		void write_gcode_file(char* output_filename);
};

void gcode_document::add_velocity_profile(int velocity){
	velocity_profiles.push_back(velocity);
}

void gcode_document::add_gcode_dataset(double X, double Y, double F, double S){
	gcode_data[0].push_back(X);
	gcode_data[1].push_back(Y);	
	gcode_data[2].push_back(F);
	gcode_data[3].push_back(S);
}

int string_to_int(const char* string){
	int val;
	std::stringstream buffer(string);
	buffer >> val;
	return val;	
}

double string_to_double(const char* string){
	double val;
	std::stringstream buffer(string);
	buffer >> val;
	return val;	
}

void gcode_document::modify_work_offset(double x, double y){

}

void gcode_document::set_work_offset(double x, double y){
	work_offset[0] = x;
	work_offset[1] = y;	
}

void gcode_document::write_gcode_file(char* output_filename){
	std::ofstream gcode_file;
	gcode_file.open(output_filename);
	
	for (int i = 0; i < gcode_data[0].size(); i++){
		gcode_file << "G1 X" << gcode_data[0][i] << " Y" << gcode_data[1][i];
		if ((i >= 1) && (gcode_data[2][i] != gcode_data[2][i-1])){
			gcode_file << " F" << gcode_data[2][i];
		}
		if ((i >= 1) && (gcode_data[2][i] != gcode_data[2][i-1])){
			gcode_file << " S" << gcode_data[3][i];
		}
		gcode_file << "\n";
	}
	gcode_file.close();
}


int main(int argc, char* argv[]){
	
	if ((argv[1] == NULL) || (argv[2] == NULL)){
	std::cout << "Error: input and output file paths not specified properly \n";
	  return 0;	  
	}
  
	std::ifstream input(argv[1]);  
	if (!input){
		std::cout << "Error: input file is not valid \n";
		return 0;
	}
	input.close();

	pugi::xml_document doc;

	pugi::xml_parse_result result = doc.load_file(argv[1]);

	gcode_document gcode_doc;
	
	std::cout << "Reading XML file... \n";
	//Get velocity profiles
	pugi::xml_node vel_prof_list = doc.child("Build").child("VelocityProfileList");
	if (vel_prof_list){
		std::cout << "Velocity Profiles found... \n";
		for (pugi::xml_node vel_prof = vel_prof_list.child("VelocityProfile"); vel_prof; vel_prof = vel_prof.next_sibling("VelocityProfile")){
			int velocity_val;
			//std::cout << "Velocity Profile " << vel_prof.child_value("ID") << ": " << vel_prof.child_value("Velocity") << std::endl;
			gcode_doc.add_velocity_profile(string_to_int(vel_prof.child_value("Velocity")));
		}
	}
	
/*
	//Initialize work offset with the "Start" Position
	pugi::xml_node initial_pos = doc.child("Build").child("Trajectory").child("Path").child("Start");
	if (initial_pos){
		gcode_doc.set_work_offset(string_to_double(initial_pos.child_value("X")), string_to_double(initial_pos.child_value("Y")));	
		std::cout << "X offset: " << gcode_doc.work_offset[0] << "   Y offset: " << gcode_doc.work_offset[1] << std::endl;
	}
	else{
		std::cout << "Error: Initial coordinates not found \n";
		return 0;
	}
*/
	std::cout << "Reading linear move segments from XML file... \n";
	//Get all movements
	pugi::xml_node xml_path_list = doc.child("Build").child("Trajectory");
	int count = 0;
	double x_coord, y_coord, laser_power, scan_speed;
	for (pugi::xml_node xml_path = xml_path_list.child("Path"); xml_path; xml_path = xml_path.next_sibling("Path")){
		//Move to the start position (need to add rapid feedrate in for paths after the first one)
		x_coord = string_to_double(xml_path.child("Start").child_value("X"));
		y_coord = string_to_double(xml_path.child("Start").child_value("Y"));
		//The "rapid" speed is set to the max velocity profile, for now
		scan_speed = *std::max_element(gcode_doc.velocity_profiles.begin(), gcode_doc.velocity_profiles.end());
		laser_power = 0;
		gcode_doc.add_gcode_dataset(x_coord, y_coord, scan_speed, laser_power);
		//std::cout << "Rapid Scan Speed: " << scan_speed << std::endl;		
		for (pugi::xml_node xml_line = xml_path.child("Segment"); xml_line; xml_line = xml_line.next_sibling("Segment")){
			x_coord = string_to_double(xml_line.child("End").child_value("X"));
			y_coord = string_to_double(xml_line.child("End").child_value("Y"));
			scan_speed = gcode_doc.velocity_profiles[string_to_int(xml_line.child_value("idxVelocityProfile"))];
			laser_power = string_to_double(xml_line.child_value("Power"));			 
			gcode_doc.add_gcode_dataset(x_coord, y_coord, scan_speed, laser_power);
			/*
			std::cout << "X: " << gcode_doc.gcode_data[0][count]
					  << "  Y: " << gcode_doc.gcode_data[1][count]
					  << "  F: " << gcode_doc.gcode_data[2][count]
					  << "  S: " << gcode_doc.gcode_data[3][count] << std::endl;
			*/
			count++;
		}
	}
	
	std::cout << "XML path file with " << count << " segments successfully read \n";
	std::cout << "Writing gcode file... \n";
	
	//Write Gcode File
	gcode_doc.write_gcode_file(argv[2]);
	std::cout << "Gcode file complete\n";	
}
