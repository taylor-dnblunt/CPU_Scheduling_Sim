#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simcpu.h"

void flag_checker(int argc, char * argv[], int * flag_arr) {

	printf("FLAG_CHECKER\n");

	for (int i = 1; i < argc; i++) {
		if (strcmp("-d", argv[i]) == 0) {
			printf("set detailed mode\n");
			flag_arr[0] = 1;
		}
		if (strcmp("-v", argv[i]) == 0) {
			printf("set verbose mode\n");
			flag_arr[1] = 1;
		}
		if (strcmp("-r", argv[i]) == 0) {
			printf("Round robin schedling with time quantum = ");
			flag_arr[2] = 1;
			printf("%d\n", atoi(argv[argc - 1]));
		}
	}
}

//Take line and return numbers from line in the num_arr
void line_parse(char * line, int * num_arr) {

	const char s[2] = " ";
	int i = 0;
	for (char * p = strtok(line, s); p != NULL; p = strtok(NULL, s)) {
		//printf("%d ", atoi(p));
		num_arr[i] = atoi(p);
		i++;
	}
}

void free_mem(sim_cont * sim){
	//Need to free lists before freeing sim
	for(int i = 0; i < sim->process; i++) {

		for(int j = 0; j < sim->proc_list[i].tnum;j++) {
			free(sim->proc_list[i].t_list[j].b_list); //Free burst lists in t_list
			
		}
			free(sim->proc_list[i].t_list); //Free t_list
	}
	free(sim); //Free sim after burst list and t list are freed
}