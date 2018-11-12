/* FEM_solution_manipulation.cpp
 *
 *
 *
 *
 *
 */

#include<iostream>
#include<string>
#include<sstream>
#include<fstream>
#include<vector>
#include<cmath>
#include<iomanip>

#include "read_results_file.hpp"
#include "node_location_tests.hpp"
#include "FEM_solution_manipulation.hpp"

using namespace std;

void create_single_pt_temp_hist_file(std::string input_filepath_pvd, std::string input_filename_base, std::string output_filename, int node_id, int step_size){
	//Create local vars
	double timestep;
	int solution_step = 0;
	std::string line_pvd; //holds the string value of the current line being read from the pvd results file "list"
	
	//Open the results file "list"
	std::ifstream input_pvd((input_filepath_pvd + input_filename_base + ".pvd").c_str());
	if (!input_pvd){std::cerr << "Error: Specified input file could not be opened!\n"; return;}
	
	std::getline(input_pvd, line_pvd);
	while (line_pvd.find("<DataSet") > 5000){
		std::getline(input_pvd, line_pvd);
	}
	
	//Create and open the output file
	ofstream outfile_temp_hist(output_filename.c_str(), ofstream::out | ofstream::trunc);
	outfile_temp_hist << "Time (seconds), Average Point Temperature (K)" << endl;
	std::ostringstream ss_pvd;
	
	//Iterate through each timestep and extract the temperature 
	int step_iter = 0;
	
	while (line_pvd.find("</Collection>") > 5000){
		step_iter++;
		if (step_iter >= step_size){
			step_iter = 0; 
			ss_pvd.str("");
			ss_pvd << solution_step;
			string input_folder = input_filepath_pvd + input_filename_base + "_" + ss_pvd.str() + "/";
			get_FEM_vtu_data(input_folder, solution_step, "Solution",  true, node_id);   
      
			sscanf(line_pvd.c_str(), "     <DataSet timestep=\"%lf", &timestep);
			outfile_temp_hist << fixed << setprecision(7) << timestep << "," <<  nodelist[0].temperature << endl;
		}
		std::getline(input_pvd,line_pvd);
		nodelist.clear();
		elementlist.clear();
		solution_step++;
	}
	outfile_temp_hist.close();
	input_pvd.close();
}


void create_thermocouple_temp_hist_file(std::string input_filepath_pvd, std::string input_filename_base, std::string output_filename, int step_size){
	//Create list to hold regular grid points
	std::vector<node> pointlist;
	
	//Create local vars
	double timestep;
	int solution_step = 1; //Non-resetting iterator that keeps track of current timestep
	std::string line_pvd;
	bool pointlist_init = false; //Must complete one solution grab before initializing the pointlist

	//Open the results file "list"
	std::ifstream input_pvd((input_filepath_pvd + input_filename_base + ".pvd").c_str()); 
	if (!input_pvd){std::cerr << "Error: Specified input file could not be opened!\n"; return;}
	
	std::getline(input_pvd, line_pvd);
	while (line_pvd.find("<DataSet") > 5000){
	  std::getline(input_pvd, line_pvd);
	}
	
	//Create and open the output file
	ofstream outfile_temp_hist(output_filename.c_str(), ofstream::out | ofstream::trunc);
	outfile_temp_hist << "Temperature History for Simulation \"" << input_filename_base << "\" " << endl; 
	outfile_temp_hist << "Time (seconds), Average Temperature at Points (K)" << std::endl; 

	//Start parsing the lines of the pvd file
	std::ostringstream ss_pvd;
	int step_iterator = 1;
	while (line_pvd.find("</Collection>") > 5000) //Before reaching the end of the list
	{
	  if (step_iterator >= step_size) //Only get data from certain timesteps 
		{
		  step_iterator = 0; 
		  ss_pvd.str("");
		  ss_pvd << solution_step;
		  //Find the folder for the mesh part vtu files for this specific timestep
		  std::string input_folder = input_filepath_pvd + input_filename_base + "_" + ss_pvd.str() + "/";
		  //Retrieve data from these vtu results files
		  get_FEM_vtu_data(input_folder, solution_step, "Solution", false, 0); 
		  //If this is the first timestep, create the list of nodes at the thermocouple junction
		  if (!pointlist_init){
			for (int nd = 0; nd < nodelist.size(); nd++){
				if ( (pow((nodelist[nd].x - (-0.000025)),2) + pow((nodelist[nd].z - (0.000150)),2) <= pow(0.000025,2)) && (nodelist[nd].y > 0.000624) && (nodelist[nd].y < 0.000626)){
					pointlist.push_back(nodelist[nd]);
				}
			}
		  cout << endl << endl << "Initialzing Thermocouple Interface Nodes Subset" << endl << endl;
		  cout << "Thermocouple Node Subset Info: " << endl << endl;
		  outfile_temp_hist << "Temperature History for Simulation \"" << input_filename_base << "\" " << endl; 
		  outfile_temp_hist << "Time (seconds), Average Temperature at Points "; 
		  for (int pt_ct = 0; pt_ct < pointlist.size(); pt_ct++ )
		  {
			outfile_temp_hist << pointlist[pt_ct].id; 
			if (pt_ct != pointlist.size()-1) outfile_temp_hist << ", ";
			pointlist[pt_ct].printinfo();
		  }
		  outfile_temp_hist << endl;
		  pointlist_init = true;   
		}
		double avg_val = 0;
		for (int pt_ct = 0 ; pt_ct < pointlist.size() ; pt_ct++)
		{
		  avg_val += pointlist[pt_ct].temperature;
		}  
		avg_val = avg_val/(pointlist.size());
		//cout << line_pvd << endl;
		sscanf(line_pvd.c_str(), "     <DataSet timestep=\"%lf", &timestep);
		outfile_temp_hist << fixed << setprecision(7) << timestep << "," <<  avg_val << endl;
	  }
	  std::getline(input_pvd,line_pvd);
	  nodelist.clear();
	  elementlist.clear();
	  solution_step++;
	  step_iterator++;
	}
	outfile_temp_hist.close();
	input_pvd.close();
}

//num_pts is an array for number of points in x,y,z directions
//box_length is an array for total lengths of the "box" in x,y,z directions
void create_grid_csv_temp_hist_file(std::string input_filepath_pvd, std::string input_filename_base, std::string output_filename, int step_size, 
									double *grid_origin, int *grid_dir, int *num_pts, double *grid_spacing){
	//Create list to hold regular grid points
	std::vector<node> pointlist;
	
	//Create local vars
	double timestep;
	int solution_step = 0; //Non-resetting iterator that keeps track of current timestep
	std::string line_pvd;
	bool pointlist_init = false; //Must complete one solution grab before initializing the pointlist

	//Open the results file "list"
	cout << "filename: " << input_filepath_pvd << input_filename_base << std::endl;
	std::ifstream input_pvd((input_filepath_pvd + input_filename_base + ".pvd").c_str());   
	if (!input_pvd){std::cerr << "Error: Specified input file could not be opened!\n"; return;}	
	std::getline(input_pvd, line_pvd);
	while (line_pvd.find("<DataSet") > 5000){
	  std::getline(input_pvd, line_pvd);
	}
	
	//Create and open the output file
	ofstream outfile_temp_hist(output_filename.c_str(), ofstream::out | ofstream::trunc);
 
	//Compute the points in the regular grid and add to the pointlist  
	cout << endl << endl << "Initialzing Regular Grid Pointlist" << endl << endl;
	int total_pt_count = 0;
	for (int x_iter = 0; x_iter < num_pts[0]; x_iter++){
		for (int y_iter = 0; y_iter < num_pts[1]; y_iter++){
			for (int z_iter = 0; z_iter < num_pts[2]; z_iter++){
				pointlist.push_back(node((grid_origin[0]+(grid_dir[0]*x_iter*grid_spacing[0])), 
										(grid_origin[1]+(grid_dir[1]*y_iter*grid_spacing[1])), 
										(grid_origin[2]+(grid_dir[2]*z_iter*grid_spacing[2])), (total_pt_count), 0));
				total_pt_count++;
			}
		}
	} 
  
	outfile_temp_hist << "Regular Grid Temperature History for Simulation \"" << input_filename_base << "\" " << endl; 
    outfile_temp_hist << "Regular Grid Point Number, X Coord, Y Coord, Z Coord" << "\n";
      
	for (int pt_ct = 0; pt_ct < pointlist.size(); pt_ct++){
		outfile_temp_hist << pointlist[pt_ct].id << ", " << pointlist[pt_ct].x << ", " << pointlist[pt_ct].y << ", " << pointlist[pt_ct].z << "\n"; 
	}

  outfile_temp_hist << "Time (seconds), Temperature at Regular Grid Points \n"; 

  std::ostringstream ss_pvd;
  int step_iterator = 0;

  while (line_pvd.find("</Collection>") > 5000){ //Iterate until the end of the simulation timesteps 
    if (step_iterator >= step_size){ //If the next step has been reached
      step_iterator = 0; 
      ss_pvd.str("");
      ss_pvd << solution_step; 
      string input_folder = input_filepath_pvd + input_filename_base + "_" + ss_pvd.str() + "/";
	  std::cout << "input vtu folder: " << input_folder << std::endl;
      //Read the results files and enter the info into the node and element data structures
      get_FEM_vtu_data(input_folder, solution_step, "Solution", false, 0); 

	  //Create and init. array to keep track and make sure all regular grid points are contained in the FEM unstructured mesh
      int interpreted_value_found[pointlist.size()]; 
	  for (int pt = 0; pt < pointlist.size(); pt++){
		interpreted_value_found[pt] = 0;
	  }

      //Create coordinate arrays for all result mesh nodes
      std::cout << "Creating coordinate arrays for nodes \n\n";
      for (int nd = 0; nd < nodelist.size(); nd++){
        nodelist[nd].create_coordinate_array();
      }
      
      //Create coordinate arrays for all receiving regular grid points
      std::cout << "Creating coordinate arrays for regular grid points \n\n";
      for (int pt = 0; pt < pointlist.size(); pt++){
        pointlist[pt].create_coordinate_array();
      }
	  
      std::cout << "Finding bounding boxes of elements \n\n";
      //Find the bounding box around all result mesh elements
      for (int elem = 0; elem < elementlist.size(); elem++){
        elementlist[elem].find_bounds();
      }

      std::cout << "Performing Interpolation... \n\n";
      //Iterate through all donor elements and find points that lie within each element
      for (int elem = 0; elem < elementlist.size(); elem++){
        for (int pt = 0; pt < pointlist.size(); pt++){
          if (elementlist[elem].is_point_inside(&pointlist[pt])){
            interpreted_value_found[pt] = 1;
            pointlist[pt].temperature = elementlist[elem].interpret_pt_value(&pointlist[pt]);
          }
        }
      }
      std::cout << "Interpolation complete \n\n";

      //Check that all points were contained inside the mesh
      bool error_msg = false;
      for (int pt = 0; pt < pointlist.size(); pt++){
        if (!interpreted_value_found[pt]) error_msg = true;
      }
      if (error_msg == true) printf("Error: Not all regular grid points are located inside the mesh! \n\n");

//    outfile_temp_hist.close();
      //    input_pvd.close();
      //    return;    
      
      //Print the results to the output file 
      sscanf(line_pvd.c_str(), "     <DataSet timestep=\"%lf", &timestep);
      
      outfile_temp_hist << fixed << setprecision(7) << timestep << ",";
  
      for (int pt = 0; pt < pointlist.size(); pt++){
        outfile_temp_hist << pointlist[pt].temperature;
        if (pt != pointlist.size() - 1) outfile_temp_hist << ",";
      }     
      outfile_temp_hist << "\n";
      nodelist.clear();
      elementlist.clear();
    }
    std::getline(input_pvd,line_pvd);
    solution_step++;
    step_iterator++;
  }
  outfile_temp_hist.close();
  input_pvd.close();

}

void create_grid_vtu_temp_hist_file(std::string input_filepath_pvd, std::string input_filename_base, std::string output_filename, int step_size, 
									double *grid_origin, int *grid_dir, int *num_pts, double *grid_spacing){	
	std::vector<node> pointlist;
	std::vector<element> output_elementlist;
	std::vector<cube> cubelist;	
	
	double timestep;
	int solution_step = 1;
	std::string line_pvd;

	std::ifstream input_pvd((input_filepath_pvd + input_filename_base + ".pvd").c_str()); 
	if (!input_pvd){std::cerr << "Error: Specified input file could not be opened!\n"; return;}
	std::getline(input_pvd, line_pvd);
	while (line_pvd.find("<DataSet") > 5000){ //while the line containing "dataset" has not been found
		std::getline(input_pvd, line_pvd);
	}
  
	ofstream outfile_temp_hist(output_filename.c_str(), ofstream::out | ofstream::trunc);
	/*
	outfile_temp_hist << "Time (seconds), Average Temperature at Points "; 
    for (int pt_ct = 0; pt_ct < pointlist.size(); pt_ct++ )
    {
      outfile_temp_hist << pointlist[pt_ct]; 
      if (pt_ct != pointlist.size()-1) outfile_temp_hist << ", ";
    }
    outfile_temp_hist << endl;
	*/    

	//Find the 8 points associated with each cube of the grid
	int total_pt_count = 0;
	int total_cube_count = 0;
	for (int x_iter = 0; x_iter < num_pts[0]; x_iter++){
    for (int y_iter = 0; y_iter < num_pts[1]; y_iter++){
		for (int z_iter = 0; z_iter < num_pts[2]; z_iter++){
			if ((x_iter < (num_pts[0]-1)) && (y_iter < (num_pts[1]-1)) && (z_iter < (num_pts[2]-1))){
				int id_list[8];
				id_list[0] = total_pt_count;  //cube origin
				id_list[1] = total_pt_count + 1;   //+z
				id_list[2] = total_pt_count + num_pts[2] + 1;  //+z+y
				id_list[3] = total_pt_count + (num_pts[2]); //+y
				id_list[4] = total_pt_count + (num_pts[2] * num_pts[1]); //+x
				id_list[5] = total_pt_count + (num_pts[2] * num_pts[1]) + 1; //+x+z	
				id_list[6] = total_pt_count + (num_pts[2] * num_pts[1]) + num_pts[2] + 1; //+x+y+z			
				id_list[7] = total_pt_count + (num_pts[2] * num_pts[1]) + num_pts[2]; //+x+y				
			
				std::cout << "Box Corner IDs: ";
				for (int iter = 0; iter < 8; iter++){
					std::cout << id_list[iter] << ", ";
				}
				std::cout << "\n";
				cubelist.push_back(cube(id_list));
				total_cube_count++;
		}
		total_pt_count++;
	  }
    }
  } 
  
  //Split the grid cubes into 6 tetrahedrons each
  for (int cube_num = 0; cube_num < total_cube_count; cube_num++){
	  output_elementlist.push_back(element(cubelist[cube_num].id_list[0], cubelist[cube_num].id_list[1], cubelist[cube_num].id_list[3], cubelist[cube_num].id_list[7], 0, (6*cube_num), 0));
	  output_elementlist.push_back(element(cubelist[cube_num].id_list[0], cubelist[cube_num].id_list[1], cubelist[cube_num].id_list[4], cubelist[cube_num].id_list[7], 1, (6*cube_num), 0)); 
	  output_elementlist.push_back(element(cubelist[cube_num].id_list[1], cubelist[cube_num].id_list[2], cubelist[cube_num].id_list[3], cubelist[cube_num].id_list[7], 2, (6*cube_num), 0));
	  output_elementlist.push_back(element(cubelist[cube_num].id_list[1], cubelist[cube_num].id_list[2], cubelist[cube_num].id_list[6], cubelist[cube_num].id_list[7], 3, (6*cube_num), 0));
	  output_elementlist.push_back(element(cubelist[cube_num].id_list[1], cubelist[cube_num].id_list[4], cubelist[cube_num].id_list[5], cubelist[cube_num].id_list[7], 4, (6*cube_num), 0));
	  output_elementlist.push_back(element(cubelist[cube_num].id_list[1], cubelist[cube_num].id_list[5], cubelist[cube_num].id_list[6], cubelist[cube_num].id_list[7], 5, (6*cube_num), 0));
 }
      
  //Initialize regular grid pointlist to receive temperature interpolation  
  cout << endl << endl << "Initialzing Regular Grid Pointlist" << endl << endl;
  total_pt_count = 0;
  for (int x_iter = 0; x_iter < num_pts[0]; x_iter++){
    for (int y_iter = 0; y_iter < num_pts[1]; y_iter++){
      for (int z_iter = 0; z_iter < num_pts[2]; z_iter++){
        pointlist.push_back(node((grid_origin[0]+(grid_dir[0]*x_iter*grid_spacing[0])), (grid_origin[1]+(grid_dir[1]*y_iter*grid_spacing[1])), (grid_origin[2]+(grid_dir[2]*z_iter*grid_spacing[2])), (total_pt_count), 0));
      total_pt_count++;
      }
    }
  }   

  //Write the point coordinates to the file
  outfile_temp_hist << "<VTKFile type=\"UnstructuredGrid\">\n<UnstructuredGrid>" << "\n" << "<Piece NumberOfPoints=\"" << (num_pts[0]*num_pts[1]*num_pts[2])
						<< "\" NumberOfCells=\"" << (output_elementlist.size()) << "\">" << "\n" << "<Points>" << "\n" 
						<< "<DataArray type=\"Float64\" Name=\"coordinates\" NumberOfComponents=\"3\" format=\"ascii\">" << "\n";  
    
  for (int pt_ct = 0; pt_ct < pointlist.size(); pt_ct++){
    outfile_temp_hist << pointlist[pt_ct].x << " " << pointlist[pt_ct].y << " " << pointlist[pt_ct].z << " \n"; 
  }
  
  //Write the point connectivity to the file
  outfile_temp_hist << "</DataArray>" << "\n" << "</Points>" << "\n" << "<Cells>" << "\n" << "<DataArray type=\"Int32\" Name=\"connectivity\" format=\"ascii\">" << "\n";
  
   for (int elem = 0; elem < output_elementlist.size(); elem++){
    outfile_temp_hist << output_elementlist[elem].v1 << " " << output_elementlist[elem].v2 << " " << output_elementlist[elem].v3 << " " << output_elementlist[elem].v4 << " \n"; 
  }
  
  //Write the offsets to the file
	outfile_temp_hist << "</DataArray>" << "\n" "<DataArray type=\"Int32\" Name=\"offsets\" format=\"ascii\">" << "\n";
   for (int elem = 1; elem <= output_elementlist.size(); elem++){
    outfile_temp_hist << (elem*4) << "\n"; 
  }

  //Write the type to the file
	outfile_temp_hist << "</DataArray>" << "\n" "<DataArray type=\"UInt8\" Name=\"types\" format=\"ascii\">" << "\n";
   for (int elem = 0; elem < output_elementlist.size(); elem++){
    outfile_temp_hist << "10" << "\n"; 
  }
   
	std::ostringstream ss_pvd;

	int step_iterator = 0;
	while (line_pvd.find("</Collection>") > 5000){ //Iterate until the end of the mesh results file is reached 
		if (step_iterator >= step_size){ //If the next step has been reached
		  step_iterator = 0; 
		  ss_pvd.str("");
		  ss_pvd << solution_step; 
		  string input_folder = input_filepath_pvd + input_filename_base + "_" + ss_pvd.str() + "/";
		  //Read the results files and enter the info into the node and element data structures
		  get_FEM_vtu_data(input_folder, solution_step, "Solution", false, 0); 
	  
		  //Create and init. array to keep track and make sure all regular grid points are contained in the FEM unstructured mesh
		  int interpreted_value_found[pointlist.size()]; 
		  for (int pt = 0; pt < pointlist.size(); pt++){
			interpreted_value_found[pt] = 0;
		  }
		  //Create coordinate arrays for all result mesh nodes
		  std::cout << "Creating coordinate arrays for nodes... \n\n";
		  for (int nd = 0; nd < nodelist.size(); nd++){
			nodelist[nd].create_coordinate_array();
		  }
		  
		  //Create coordinate arrays for all receiving regular grid points
		  std::cout << "Creating coordinate arrays for reg grid points... \n\n";
		  for (int pt = 0; pt < pointlist.size(); pt++){
			pointlist[pt].create_coordinate_array();
		  }

		  //Find the bounding box around all input mesh elements
		  std::cout << "Finding bounding boxes of elements... \n\n";
		  for (int elem = 0; elem < elementlist.size(); elem++){
			elementlist[elem].find_bounds();
		  }
		  
		  //Iterate through all donor elements and find points that lie within each element
		  std::cout << "Interpolation solution onto regular grid... \n\n";
		  for (int elem = 0; elem < elementlist.size(); elem++){
			for (int pt = 0; pt < pointlist.size(); pt++){
			  if (elementlist[elem].is_point_inside(&pointlist[pt])){
				interpreted_value_found[pt] = 1;
				pointlist[pt].temperature = elementlist[elem].interpret_pt_value(&pointlist[pt]);
			  }
			}
		  }
		  std::cout << "Interpolation complete \n\n";

		  //Check that all points were contained inside the mesh
		  bool error_msg = false;
		  for (int pt = 0; pt < pointlist.size(); pt++){
			if (!interpreted_value_found[pt]) error_msg = true;
		  }
		  if (error_msg == true){
			  printf("Error: Not all regular grid points are located inside the mesh! \n\n"); 
			  return;
		  }

		  
		  //Write the data to the file
		  outfile_temp_hist << "</DataArray>" << "\n" << "</Cells>" << "\n" << "<PointData>" << "\n" << "<DataArray type=\"Float64\" Name=\"Solution\" NumberOfComponents=\"1\" format=\"ascii\">" << "\n";
		  for (int pt = 0; pt < pointlist.size(); pt++){
			outfile_temp_hist << pointlist[pt].temperature << " \n";
		  }    
		  
		  //Close out the file
		  outfile_temp_hist << "</DataArray>\n</PointData>\n</Piece>\n</UnstructuredGrid>\n</VTKFile>";	  
		  
		  nodelist.clear();
		  elementlist.clear();
		}
		std::getline(input_pvd,line_pvd);
		solution_step++;
		step_iterator++;
	}
  
  outfile_temp_hist.close();
  input_pvd.close();

}

