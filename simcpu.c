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
	int burstFlag = 0;//True if currently counting cpu bursts
	int threadFlag = 0;//True if last thread in process completes its final cpu burst
	int lineCnt = 0;
	int burstCnt = 0;
	int numBursts = 0;
	int threadCnt = 0;//use to count the current thread number in the input file
	int numThreads = 0;

	//Add processes to list in sim
	sim_cont * sim = (sim_cont *)malloc(sizeof(struct sim_cont));
	

	//This while loop goes through input and gathers all important data
	while(nread = getline(&line, &length, stdin)!= -1) {
		lineCnt++;
		strcpy(backupLine, line);
		line_parse(line, nums);

		if (lineCnt == 1) {//Initial info
			sim->process = nums[0];
			sim->same_switch = nums[1];
			sim->dif_switch = nums[2];

		}
		if (lineCnt == 2) {//First process
			proc * p = (proc *)malloc(sizeof(struct process));
			p->tnum = nums[1];
			numThreads = nums[1];
			//FIXME:
			//Make an array that contains enough space for the threads in process 1
			t_type * t = malloc(p->tnum * sizeof(struct threads *));
			p->array = t;
			/*for (int i = 0; i < numThreads; i++) {
				//FIXME: trying to assign space for each thread in the array of threads
				((p->array)[i]) = (t_type *)malloc(sizeof((p->array)[i]));
			}*/
			printf("num threads in first process = %d\n", p->tnum);
		}
		if (lineCnt == 3) {//Thread and cpu burst
			threadCnt++;
			burstFlag = 1;
			t_type * t = (t_type *)malloc(sizeof(t_type));
			t->arrive = nums[1];
			t->cpu_bursts = nums[2];
			numBursts = nums[2];
			printf("thread 1 arrives %d and has %d cpu bursts\n", t->arrive, t->cpu_bursts);


		//First three lines are always the same so below checks for the remaining lines	
		} else if (burstFlag == 1 && burstCnt == numBursts) {//Reset for new thread info
			threadFlag = 0;
			burstCnt = 0;
			//printf("threadCnt = %d and numThreads = %d\n", threadCnt, numThreads);
			if (threadCnt < numThreads) {
				burstFlag = 1;
				threadCnt++;
				numBursts = nums[2];
				//printf("threadCnt = %d\n", threadCnt);
				printf("This line is the next thread in a process: ");
				printf("%sand has %d bursts\n", backupLine, numBursts);
			} else if (threadCnt == numThreads) {
				//Next line is a new process or the end
				//Found new process or end so if not end and there are numbers take the 
				//last number as the new threadNum
				printf("This line is a process: ");
				printf("%s", backupLine);
				numThreads = nums[1];
				threadFlag = 1;
				burstFlag = 0;
				printf("num threads in this process = %d\n", numThreads);
				
			}


		} else if (burstFlag == 1) { //Go the certain number of cpu bursts then grab next thread
			burstCnt++;
			printf("This line is a burst: %s", backupLine);
		} else if (threadFlag == 1) { //Next line is new thread info
			printf("This line is a new thread: ");
			printf("%s\n", backupLine);
			numBursts = nums[2];
			printf("numBursts = %d and burstCnt = %d\n", numBursts, burstCnt);
			burstFlag = 1;
			threadCnt = 1;
		}

		//Go through the lines of the cpu bursts then grab the next thread
	}
	printf("\n");
	printf("Line count = %d\n", lineCnt);
	printf("p = %d, ss = %d, ds = %d\n", sim->process, sim->same_switch, sim->dif_switch);
	free(sim);
}
