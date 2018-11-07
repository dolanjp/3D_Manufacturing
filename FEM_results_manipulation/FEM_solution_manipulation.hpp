/* FEM_solution_manipulation.hpp
 *
 *
 *
 *
 *
 */

#ifndef FEM_SOLUTION_MANIPULATION_HPP
#define FEM_SOLUTION_MANIPULATION_HPP
#include<string>
#include<sstream>

void create_single_pt_temp_hist_file(std::string input_filepath_pvd, std::string input_filename_base, std::string output_filename, int node_id, int step_size);

void create_thermocouple_temp_hist_file(std::string input_filepath_pvd, std::string input_filename_base, std::string output_filename, int step_size);

//regular grid csv file outputs for input to phase field model
void create_grid_csv_temp_hist_file(std::string input_filepath_pvd, std::string input_filename_base, std::string output_filename, int step_size, 
									double *grid_origin, int *grid_dir, int *num_pts, double *grid_spacing);

//regular grid vtu file outputs to view results in Paraview
void create_grid_vtu_temp_hist_file(std::string input_filepath_pvd, std::string input_filename_base, std::string output_filename, int step_size, 
									double *grid_origin, int *grid_dir, int *num_pts, double *grid_spacing);


#endif