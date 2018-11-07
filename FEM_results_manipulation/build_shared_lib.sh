#!/bin/bash

gcc -c -Wall -Werror -fpic _3DM_solution_manipulation.c
gcc -shared -o lib_3DM_solution_manipulation.so _3DM_solution_manipulation.o




  