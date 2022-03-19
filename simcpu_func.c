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

void set_proc(sim_cont * sim, int * nums) {
	proc * p = malloc(nums[0] * sizeof *p);
	sim->proc_list = p;//Points to the beginning of the array containing the processes
	sim->process = nums[0];
	sim->same_switch = nums[1];
	sim->dif_switch = nums[2];
}

void set_init_proc(sim_cont * sim, int * nums) {
	int numThreads = nums[1];
	sim->proc_list[0].tnum = numThreads;
	sim->proc_list[0].cur_tnum = 0;
	sim->proc_list[0].threads_terminated = 0;

	//Make an array that contains enough space for the threads in process 1
	t_type * t = malloc(numThreads * sizeof *t);
	sim->proc_list[0].t_list = t;
	
	printf("num threads in first process = %d\n", sim->proc_list[0].tnum);
}

void set_init_thread(sim_cont * sim, int * nums) {
	//Set arrival and number of cpu bursts
	int numBursts = nums[2];
	sim->proc_list[0].t_list[0].arrive = nums[1];
	sim->proc_list[0].t_list[0].cpu_bursts = numBursts;
	//Allocate the amount of bursts
	burst * b = malloc(numBursts * sizeof *b);
	sim->proc_list[0].t_list[0].b_list = b;
	sim->proc_list[0].t_list[0].cur_b = 0;
	sim->proc_list[0].t_list[0].parent_process = 0;
}

void set_new_thread(sim_cont * sim, int * nums) {
	int numBursts = nums[2];
	int arrival = nums[1];
	//Malloc space for the amount of bursts in each thread
	burst * b = malloc(numBursts * sizeof *b);
	sim->proc_list[sim->cur_proc].cur_tnum++;
	int cur_thread = sim->proc_list[sim->cur_proc].cur_tnum;
	sim->proc_list[sim->cur_proc].t_list[cur_thread].b_list = b;
	sim->proc_list[sim->cur_proc].t_list[cur_thread].cur_b = 0;
	sim->proc_list[sim->cur_proc].t_list[cur_thread].arrive = arrival;
	sim->proc_list[sim->cur_proc].t_list[cur_thread].cpu_bursts = numBursts;
	sim->proc_list[sim->cur_proc].t_list[cur_thread].parent_process = sim->cur_proc;
}

void set_new_proc(sim_cont * sim, int * nums) {
	sim->cur_proc++;
	sim->proc_list[sim->cur_proc].tnum = nums[1];
	int threads = nums[1];
	sim->proc_list[sim->cur_proc].cur_tnum = 0;
	sim->proc_list[sim->cur_proc].threads_terminated = 0;
	//Malloc space for thread array
	t_type * t = malloc(threads * sizeof *t);
	sim->proc_list[sim->cur_proc].t_list = t;
}

void set_new_burst(sim_cont * sim, int * nums) {
	t_type * cur_thread = &sim->proc_list[sim->cur_proc]
	.t_list[sim->proc_list[sim->cur_proc].cur_tnum];
	burst * cur_burst = &((*cur_thread).b_list[(*cur_thread).cur_b]);
	// printf("cpu and io %d %d\n", nums[1], nums[2]);
	(*cur_burst).num = nums[0];
	(*cur_burst).cpu = nums[1];
	(*cur_burst).io = nums[2];
	//Not linking burst to the thread?
	//Might be setting a new instance of something thats not actually the list
	// printf("cpu of burst %d\n", cur_thread.b_list[cur_thread.cur_b].cpu);
	(*cur_thread).cur_b++;
}

void set_tAfterPLine(sim_cont * sim, int * nums) {
	int numBursts = nums[2];
	int arrival = nums[1];
	//Malloc space for the amount of bursts in each thread
	burst * b = malloc(numBursts * sizeof *b);
	int cur_thread = sim->proc_list[sim->cur_proc].cur_tnum;
	sim->proc_list[sim->cur_proc].t_list[cur_thread].b_list = b;
	sim->proc_list[sim->cur_proc].t_list[cur_thread].cur_b = 0;
	sim->proc_list[sim->cur_proc].t_list[cur_thread].cpu_bursts = numBursts;
	sim->proc_list[sim->cur_proc].t_list[cur_thread].arrive = arrival;
	//Check that parent process does the trick
	sim->proc_list[sim->cur_proc].t_list[cur_thread].parent_process = sim->cur_proc;
}