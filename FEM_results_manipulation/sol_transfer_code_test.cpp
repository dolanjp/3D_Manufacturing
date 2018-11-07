#include "FEM_solution_manipulation.hpp"
#include<cmath>

int main()
{
	//input_filepath_pvd = "../AMP_Modeling_Data/Thermocouple_Simulation/Thermocouple_Parallel_60W_160mms/";
	//input_filename_base = "center_zigzag_test_actual";
	std::string input_filepath = "/lore/dolanj/AMP_Modeling_Data/Thermocouple_Simulation/Thermocouple_Sim_Results/Thermocouple_Parallel_60W_160mms/";
	std::string input_filename = "Thermocouple_Parallel_60W_160mms";
	std::string output_filename = "sol_transfer_code_test.csv";
	int step_size = 5;
	double grid_origin[] = {50e-6, 500e-6, 0};
	double grid_opposite_corner[] = {250e-6, 700e-6, 50e-6};
	int grid_dir[] = {1,1,1};  //1 for positive direction, -1 for negative direction
	int num_pts[] = {10,10,10};
	double grid_spacing[] = {std::abs(grid_origin[0]-grid_opposite_corner[0])/((double)num_pts[0]-1),
							 std::abs(grid_origin[1] - grid_opposite_corner[1])/((double)num_pts[1]-1),
							 std::abs(grid_origin[2] - grid_opposite_corner[2])/((double)num_pts[2]-1)};

	create_grid_csv_temp_hist_file(input_filepath, input_filename, output_filename, step_size, grid_origin, grid_dir, num_pts, grid_spacing);
}