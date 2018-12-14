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

void create_single_pt_temp_hist_file(std::string input_filepath_pvd, std::string input_filename_base, int num_parts, std::string output_filename, int node_id);

void create_thermocouple_temp_hist_file(std::string input_filepath_pvd, std::string input_filename_base, int num_parts, std::string output_filename);

// Temporary function used to artificially compute the deformed configuration of the powder layer.
// This will collapse the entire powder layer uniformly, so care must be taken to only extract
// results from regions where the powder was fully melted and consolidated
void create_grid_csv_temp_hist_file_consol(std::string input_filepath_pvd, std::string input_filename_base, int num_parts, std::string output_filename,  
									double *grid_origin, int *grid_dir, int *num_pts, double *grid_spacing,
									double powder_layer_thickness, double initial_porosity);

//regular grid csv file outputs for input to phase field model
void create_grid_csv_temp_hist_file(std::string input_filepath_pvd, std::string input_filename_base, int num_parts, std::string output_filename,  
									double *grid_origin, int *grid_dir, int *num_pts, double *grid_spacing);

//regular grid vtu file outputs to view results in Paraview
void create_grid_vtu_temp_hist_file(std::string input_filepath_pvd, std::string input_filename_base, int num_parts, std::string output_filename_base,
									double *grid_origin, int *grid_dir, int *num_pts, double *grid_spacing);


									
									
#endif