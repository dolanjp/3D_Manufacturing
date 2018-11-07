#!/bin/bash

g++ -c read_results_file.cpp -o read_results_file.o
g++ -c node_location_tests.cpp -o node_location_tests.o
g++ -c FEM_solution_manipulation.cpp -o FEM_solution_manipulation.o
#g++ -c <user_filename>.cpp -o <user_filename>.o
g++ -c sol_transfer_code_test.cpp -o sol_transfer_code_test.o

g++ read_results_file.o node_location_tests.o FEM_solution_manipulation.o sol_transfer_code_test.o -o sol_transfer_code_test