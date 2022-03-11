//Taylor Blunt 0793684

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
	char * backupLine = (char *)malloc(256);
	int threadFlag = 0;//True if currently counting cpu bursts
	int processFlag = 0;//True if last thread in process completes its final cpu burst
	int lineCnt = 0;
	int burstCnt = 0;
	int numBursts = 0;
	int threadCnt = 0;//use to count the current thread number in the input file
	int numThreads = 0;
	//Get number of threads in a process
	//Get number of cpu bursts in thread
	//Go the number of lines that there are cpu bursts
	//Next line is either the new thread or the new process
	sim_cont * sim = (sim_cont *)malloc(sizeof(struct sim_cont));


	//First line is #processes / switch threads in same process / switch threads in different processes
	//This while loop goes through input and gathers all important data
	while(nread = getline(&line, &length, stdin)!= -1) {
		printf("%s", line);
		lineCnt++;
		strcpy(backupLine, line);
		line_parse(line, nums);

		if (lineCnt == 1) {//Initial info
			sim->process = nums[0];
			sim->same_switch = nums[1];
			sim->dif_switch = nums[2];

		}
		if (lineCnt == 2) {//First process
			proc p;
			p.tnum = nums[1];
			numThreads = nums[1];
			printf("num threads in first process = %d\n", p.tnum);
		}
		if (lineCnt == 3) {//Thread and cpu burst
			threadCnt++;
			threadFlag = 1;
			t_type * t = (t_type *)malloc(sizeof(t_type));
			t->arrive = nums[1];
			t->cpu_bursts = nums[2];
			numBursts = nums[2];
			printf("thread 1 arrives %d and has %d cpu bursts\n", t->arrive, t->cpu_bursts);
		} else if (threadFlag == 1 && burstCnt == numBursts) {//Reset for new thread info
			threadFlag = 0;
			burstCnt = 0;
			printf("threadCnt = %d and numThreads = %d\n", threadCnt, numThreads);
			if (threadCnt < numThreads) {
				threadFlag = 1;
				threadCnt++;
				printf("threadCnt = %d", threadCnt);
			} else if (threadCnt == numThreads) {
				//Next line is a new process or the end
				printf("This line is something: \n");
				printf("%s\n", backupLine);
			}

			printf("This line is a thread:\n");
			printf("%s\n", backupLine);
		} else if (threadFlag == 1) { //Go the certain number of cpu bursts then grab next thread
			burstCnt++;
		}

		//Go through the lines of the cpu bursts then grab the next thread
	}
	printf("Line count = %d\n", lineCnt);
	printf("p = %d, ss = %d, ds = %d\n", sim->process, sim->same_switch, sim->dif_switch);
	free(sim);
}
