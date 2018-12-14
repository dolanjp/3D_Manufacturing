#include "FEM_solution_manipulation.hpp"
/*
int main()
{
						
								
	std::string input_filepath = "/lore/dolanj/AMP_Modeling_Data/Thermocouple_Simulation/Thermocouple_Sim_Results/Thermocouple_Parallel_60W_160mms/";
	std::string input_filename = "Thermocouple_Parallel_60W_160mms";
	std::string output_filename = "sol_transfer_code_test.csv";
	double grid_origin[] = {50e-6, 500e-6, 0};
	int num_parts = 4;
	int grid_dir[] = {1,1,1};
	int num_pts[] = {10,10,1};
	double grid_spacing[] = {20e-6, 20e-6, 5e-6};
	create_grid_csv_temp_hist_file(input_filepath, input_filename, num_parts, output_filename, grid_origin, grid_dir, num_pts, grid_spacing);
}
*/
/*
int main()
{
						
								
	std::string input_filepath = "/lore/dolanj/AMP_Modeling_Data/Thermocouple_Simulation/Thermocouple_Sim_Results/Thermocouple_Parallel_60W_160mms/";
	std::string input_filename = "Thermocouple_Parallel_60W_160mms";
	std::string output_filename = "sol_transfer_code_test.csv";
	double grid_origin[] = {50e-6, 500e-6, 0};
	int num_parts = 4;
	int grid_dir[] = {1,1,1};
	int num_pts[] = {10,10,1};
	double grid_spacing[] = {20e-6, 20e-6, 5e-6};
	double powder_layer_thickness = 50e-6;
	double initial_porosity = 0.7;
		
	create_grid_csv_temp_hist_file_consol(input_filepath, input_filename, num_parts, output_filename, grid_origin, grid_dir, num_pts, grid_spacing
											powder_layer_thickness, initial_porosity);
}
*/

int main()
{
						
								
	std::string input_filepath = "/lore/dolanj/AMP_Modeling_Data/Thermocouple_Simulation/Thermocouple_Sim_Results/Thermocouple_Parallel_60W_160mms/";
	std::string input_filename = "Thermocouple_Parallel_60W_160mms";
	std::string output_filename_base = "test_12_14_thermocouple";  //without .vtu appendix
	double grid_origin[] = {136e-6, 512e-6, 32.5e-6};
	int num_parts = 4;
	int grid_dir[] = {1,1,1};
	int num_pts[] = {10,10,5};
	double grid_spacing[] = {8.3e-6, 10.8e-6, 9.9e-6};
		
	create_grid_vtu_temp_hist_file(input_filepath, input_filename, num_parts, output_filename_base, grid_origin, grid_dir, num_pts, grid_spacing);
}

/*
	double grid_opposite_corner[] = {250e-6, 700e-6, 50e-6};
	double grid_spacing[] = {std::abs(grid_origin[0]-grid_opposite_corner[0])/((double)num_pts[0]-1),
							 std::abs(grid_origin[1] - grid_opposite_corner[1])/((double)num_pts[1]-1),
							 std::abs(grid_origin[2] - grid_opposite_corner[2])/((double)num_pts[2]-1)};
					*/