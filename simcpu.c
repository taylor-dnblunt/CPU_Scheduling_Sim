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

	flag_checker(argc, argv, flags);
	for (int i = 0; i < 3; i++) {
		printf("flag[%d] = %d\n", i, flags[i]);
	}

	char * line;
	const char s[2] = " ";
	char * token;
	size_t length = 0;
	ssize_t nread = 0;
	line = (char *)malloc(256);

	while(nread = getline(&line, &length, stdin)!= -1) {
		printf("line = %s\n", line);
	}


}
