# Taylor Blunt 0793684
# Operating Systems Assignment 2

simcpu.c contains the main function along with the input parser, FCFS while loop, RR while loop,
and deals with printing the relevant flag prints.

simcpu_func.c contains the helper functions used in simcpu.c.

min_heap.c contains the min heap c code obtained from the internet. 
https://gist.github.com/sudhanshuptl/d86da25da46aa3d060e7be876bbdb343

finally simcpu.h contains the structs used for the assignment as well as the function definitions.

The program works by gathering all the information from the input.txt file, making a thread list and inserting
all threads into heaps, then, runs the chosen simulation method and flags.
