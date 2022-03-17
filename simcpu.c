//Taylor Blunt 0793684

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simcpu.h"



int main (int argc, char * argv[]) {

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
			//Makes an array containing the amount of processes and switch times
			set_proc(sim, nums);
		}
		if (lineCnt == 2) {//Initial process
			set_init_proc(sim, nums);
			numThreads = nums[1];
		}
		if (lineCnt == 3) {//Initial thread
			threadCnt++;
			burstFlag = 1;
			numBursts = nums[2];
			set_init_thread(sim, nums);
			printf("thread 1 arrives %d and has %d cpu bursts\n", sim->proc_list[0].t_list[0].arrive, 
			sim->proc_list[0].t_list[0].cpu_bursts);

		//First three lines are always the same so below checks for the remaining lines	
		} else if (burstFlag == 1 && burstCnt == numBursts) {//Reset for new thread info
			threadFlag = 0;
			burstCnt = 0;
			
			if (threadCnt < numThreads) {
				//NEW THREAD
				burstFlag = 1;
				threadCnt++;
				numBursts = nums[2];
				set_new_thread(sim, nums);

				//printf("threadCnt = %d\n", threadCnt);
				printf("This line is the next thread in a process: ");
				printf("%sand has %d bursts\n", backupLine, numBursts);

			} else if (threadCnt == numThreads) {
				//NEW PROCESS
				set_new_proc(sim, nums);

				printf("This line is a process: ");
				printf("%s", backupLine);
				numThreads = nums[1];
				threadFlag = 1;
				burstFlag = 0;
				printf("num threads in this process = %d\n", numThreads);
			}

		} else if (burstFlag == 1) {
			//NEW BURST
			burstCnt++;
			printf("This line is a burst: %s", backupLine);
			set_new_burst(sim, nums);

		} else if (threadFlag == 1) { //Next line is new thread info
			printf("This line is a new thread after a process line: ");
			printf("%s", backupLine);
			numBursts = nums[2];

			set_tAfterPLine(sim, nums);

			printf("numBursts = %d and burstCnt = %d\n", numBursts, burstCnt);
			burstFlag = 1;
			threadCnt = 1;
		}
	}
	printf("\n");
	printf("Line count = %d\n", lineCnt);
	printf("p = %d, ss = %d, ds = %d\n", sim->process, sim->same_switch, sim->dif_switch);
	printf("process 1 in the sim has %d threads and sim current process = %d\n",
	 sim->proc_list[0].tnum, sim->cur_proc);
	printf("second thread in process 1 has %d bursts\n", sim->proc_list[0].t_list[1].cpu_bursts);
	printf("first thread in process 2 has %d bursts\n", sim->proc_list[1].t_list[0].cpu_bursts);
	



	//Start making queues
	int capacity = sim->process * 50;
	printf("Capacity of heap = %d\n", capacity);
	int heap_type = 0;//for min heap
	CreateHeap(capacity, heap_type);


	//Keep running while all threads or processes are not terminated
	int threadTerminated = 0;
	int threadTot = 0;
	for (int i = 0; i < sim->process;i++) {
		threadTot += sim->proc_list[i].tnum;
	}
	printf("Thread total = %d\n", threadTot);
	// while (threadTerminated < threadTot) {

	// }

	//Deal with FCFS
	if (flags[2] != 1) {//1 being flag -r was present and a time quantum given for rr
		printf("This is the start of FCFS\n");
	}

	free_mem(sim);
}
