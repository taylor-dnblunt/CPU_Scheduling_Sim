//Taylor Blunt 0793684

#include <stdio.h>
#include <stdlib.h>
#include "simcpu.h"



int main (int argc, char * argv[]) {

	/*
	Run in FCFS if no flags are given
	Output total time, avg turnaround time of the processes
	and cpu utilization
	*/
	/*
	if -d is given (detailed mode)
	Output same as above plus arrival time, service time,
	I/O time, turnaround time, and finish time for each thread
	*/
	/*
	if -v is given (verbose mode)
	same as above plus,
	include every event that occurs, the time of the event and
	the state transition
	threads can be in new, ready, running, blocked, or terminated
	include summary info about each thread after termination
	*/
	int flags[3] = {0, 0, 0};
	int nums[3] = {0, 0, 0}; //FIXME: potential problem here because its usually 2 or 3 numbers
	

	flag_checker(argc, argv, flags);
	for (int i = 0; i < 3; i++) {
		printf("flag[%d] = %d\n", i, flags[i]);
	}

	char * line;
	size_t length = 0;
	ssize_t nread = 0;
	line = (char *)malloc(256);

	sim_cont sim;

	int lineCnt = 0;
	//First line is #processes / switch threads in same process / switch threads in different processes
	while(nread = getline(&line, &length, stdin)!= -1) {

		lineCnt++;
		line_parse(line, nums);

		if (lineCnt == 1) {//Initial info
			sim.process = nums[0];
			sim.same_switch = nums[1];
			sim.dif_switch = nums[2];

		}
		if (lineCnt == 2) {//First process
			proc p;
			p.tnum = nums[1];
			printf("num threads in first process = %d\n", p.tnum);
		}
		if (lineCnt == 3) {//Thread and cpu burst
			t_type * t;
			t->arrive = nums[1];
			t->cpu_bursts = nums[2];
			printf("thread 1 arrives %d and has %d cpu bursts\n", t->arrive, t->cpu_bursts);
		}
		//Go through the lines of the cpu bursts then grab the next thread
	}
	printf("Line count = %d\n", lineCnt);
	printf("p = %d, ss = %d, ds = %d\n", sim.process, sim.same_switch, sim.dif_switch);

}
