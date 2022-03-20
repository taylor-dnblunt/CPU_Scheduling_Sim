//Taylor Blunt 0793684

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simcpu.h"



int main (int argc, char * argv[]) {

	int flags[3] = {0, 0, 0};
	int nums[3] = {0, 0, 0};
	int quantum = 0;

	quantum = flag_checker(argc, argv, flags);
	if (quantum == -1) {
		printf("Please supply a time quantum\n");
		return 0;
	}

	char * line;
	size_t length = 0;
	ssize_t nread = 0;
	line = (char *)malloc(256);
	char * backupLine = (char *)malloc(256);
	int burstFlag = 0;//True if currently counting cpu bursts
	int threadFlag = 0;//True if last thread in process completes its final cpu burst

	/*Below is used to keep track of where the code is in the input file when extracting the data */
	int lineCnt = 0;
	int burstCnt = 0;
	int numBursts = 0;
	int threadCnt = 0;
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
			} else if (threadCnt == numThreads) {
				//NEW PROCESS
				set_new_proc(sim, nums);
				numThreads = nums[1];
				threadFlag = 1;
				burstFlag = 0;
			}

		} else if (burstFlag == 1) {
			//NEW BURST
			burstCnt++;
			set_new_burst(sim, nums);

		} else if (threadFlag == 1) { //Next line is new thread info
			//NEW THREAD AFTER PROCESS LINE
			numBursts = nums[2];
			set_tAfterPLine(sim, nums);
			burstFlag = 1;
			threadCnt = 1;
		}
	}
	
	//Start making queue
	int capacity = sim->process * 50;
	int heap_type = 0;//for min heap

	//Initialize pq
	Heap * pq = CreateHeap(capacity, heap_type);

	//Keep running while all threads or processes are not terminated
	int threadTerminated = 0;
	int threadTot = 0;
	
	//Set final burst io and double check cpu_left time
	for (int i = 0; i < sim->process; i++) {//Go through procs
		for (int j = 0; j < sim->proc_list[i].tnum; j++){//Go through threads
			for (int k = 0; k < sim->proc_list[i].t_list[j].cpu_bursts; k++) {//Go through bursts
				if (sim->proc_list[i].t_list[j].b_list[k].num == sim->proc_list[i].t_list[j].cpu_bursts) {
					//If the burst in that thread = the last burst set the io to 0
					sim->proc_list[i].t_list[j].b_list[k].io = 0;
					sim->proc_list[i].t_list[j].b_list[k].cpu_left = sim->proc_list[i].t_list[j].b_list[k].cpu;
				}
			}
		}

	}

	//Go through all threads and insert into a heap
	for (int i = 0; i < sim->process;i++) {
		threadTot += sim->proc_list[i].tnum;
		for(int j = 0; j < sim->proc_list[i].tnum;j++) {
			//This makes a new thread that has the same properties but isnt referencing the list
			t_type thread = sim->proc_list[i].t_list[j];
			//Reset cur_b so it starts on the first burst and not the last
			thread.cur_b = 0;
			insert(pq, thread);
		}
	}

	t_type thread;
	int arr_time = 0;
	int next_e = 0;
	int num_of_bursts = 0;
	int prev_proc_num = -1;
	int prev_t;
	int prev_b;
	int sim_time = 0;

	//Essentially while pq not empty i.e there are still threads that need processing
	while (threadTerminated < threadTot) {

		//This is the start of FCFS
		if (flags[2] != 1) {//1 being flag -r was present and a time quantum given for rr
			
			thread = PopMin(pq);

			//Verbose mode is set and printouts are as below
			if (thread.arrive == thread.init_arrive && flags[1] == 1) {
				printf("At time %d: Thread %d of Process %d moves from new to ready\n", thread.init_arrive, thread.thread_num + 1, thread.parent_process + 1);
			} else if (thread.arrive != thread.init_arrive && flags[1] == 1) {//Blocked to ready
				printf("At time %d: Thread %d of Process %d moves from blocked to ready\n", thread.arrive, thread.thread_num + 1, thread.parent_process + 1);
			}

			//Set thread switch times
			if (prev_proc_num != -1) {//Check to make sure its not the first thread when theres no prev proc
				if (thread.parent_process == prev_proc_num) {//Same process switch time
					sim_time += sim->same_switch;
				} else if (thread.parent_process != prev_proc_num) {//Dif process switch time
					sim_time += sim->dif_switch;
				}
			}

			if (flags[1] == 1) {
				//Enters the cpu
				printf("At time %d: Thread %d of Process %d moves from ready to running\n", sim_time, thread.thread_num + 1, thread.parent_process + 1);				
			}

			//Set when to enter the ready q again
			thread.arrive = thread.b_list[thread.cur_b].cpu + thread.b_list[thread.cur_b].io + sim_time;
			next_e = thread.b_list[thread.cur_b].cpu;

			if (flags[1] == 1) {//Verbose mode running to blocked
					printf("At time %d: Thread %d of Process %d moves from running to blocked\n", sim_time + next_e, thread.thread_num + 1, thread.parent_process + 1);
			}

			//Setup for next thread burst
			arr_time = next_e;
			sim_time += arr_time;
			prev_proc_num = thread.parent_process;
			sim->proc_list[thread.parent_process].t_list[thread.thread_num].cpu_tot += thread.b_list[thread.cur_b].cpu;
			thread.cur_b++;

			if (thread.cur_b == thread.cpu_bursts) {//This is the last burst and does
				// not go back into the queue
				threadTerminated++;
				sim->proc_list[thread.parent_process].t_list[thread.thread_num].time_finished = sim_time;
				thread.time_finished = sim_time;
				//Count up threads terminated for the process and store time once both are done
				sim->proc_list[thread.parent_process].threads_terminated++;

				if (sim->proc_list[thread.parent_process].threads_terminated == sim->proc_list[thread.parent_process].tnum) {
					//Process finish time
					sim->proc_list[thread.parent_process].time_finished = sim_time;
				}

				if (flags[1] == 1) {//Verbose mode
					thread.cpu_tot = sim->proc_list[thread.parent_process].t_list[thread.thread_num].cpu_tot;

					//Set total io used for summary
					for (int i = 0; i < sim->proc_list[thread.parent_process].t_list[thread.thread_num].cpu_bursts - 1; i++) {
						thread.io_tot += sim->proc_list[thread.parent_process].t_list[thread.thread_num].b_list[i].io;
					}
					//Give summary info
					printf("At time %d: Thread %d of Process %d moves from running to terminated\n", sim_time, thread.thread_num + 1, thread.parent_process + 1);
					printf("Arrival time: %d\n", thread.init_arrive);
					printf("Service time: %d units, IO time: %d units, turnaround time: %d units, finish time: %d\n"
					, thread.cpu_tot, thread.io_tot, (thread.time_finished - thread.init_arrive), thread.time_finished);
					printf("\n");
				}
			} else {//Theres still some cpu bursts left and it needs to go back in queue
				insert(pq, thread);	
			}
			num_of_bursts++;
			
		//This is the start of round robin
		} else if (flags[2] == 1) {//Flag -r and quantum are given

			thread = PopMin(pq);

			//Verbose mode and printouts are as below
			if (thread.arrive == thread.init_arrive && flags[1] == 1) {
				printf("At time %d: Thread %d of Process %d moves from new to ready\n", thread.init_arrive, thread.thread_num + 1, thread.parent_process + 1);
			} else if (thread.arrive != thread.init_arrive && flags[1] == 1) {//Blocked to ready
				printf("At time %d: Thread %d of Process %d moves from blocked to ready\n", thread.arrive, thread.thread_num + 1, thread.parent_process + 1);
			}
			
			//Deals with the thread switch overheads
			if (prev_proc_num != -1) {//Check to make sure its not the first thread when theres no prev proc
				//This is the same burst on the same thread
				if (thread.parent_process == prev_proc_num && thread.thread_num == prev_t && thread.cur_b == prev_b) {
					//printf("This is the same burst\n");
				} else {
					if (thread.parent_process == prev_proc_num) {//Same process switch time
						sim_time += sim->same_switch;
					} else if (thread.parent_process != prev_proc_num) {//Dif process switch time
						sim_time += sim->dif_switch;
					}
				}
			}

			//Deals with if the cpu is ready before any thread is ready
			if (thread.arrive > sim_time) {
				sim_time = thread.arrive;
			}

			//Setup for next burst arrival
			prev_proc_num = thread.parent_process;
			prev_t = thread.thread_num;
			prev_b = thread.cur_b;

			//Set verbose mode
			if (flags[1] == 1) {
				printf("At time %d: Thread %d of Process %d moves from ready to running\n", sim_time, thread.thread_num + 1, thread.parent_process + 1);
			}

			//If not done current cpu service stay and decrement time left
			if (thread.b_list[thread.cur_b].cpu_left > quantum) {//Subtract quanta time from service time 

				if (flags[1] == 1) {//Verbose mode running to blocked
					printf("At time %d: Thread %d of Process %d moves from running to blocked\n", sim_time + quantum, thread.thread_num + 1, thread.parent_process + 1);
				}
				thread.arrive = quantum + thread.b_list[thread.cur_b].io + sim_time;
				sim->proc_list[thread.parent_process].t_list[thread.thread_num].b_list[thread.cur_b].cpu_left -= quantum;
				next_e = quantum;//Supposed to pop ahead proper amount to next event

			//Else done current burst cpu service go to next in thread
			} else if (thread.b_list[thread.cur_b].cpu_left <= quantum) {//Only go service time before io
				
				if (flags[1] == 1) {//Verbose mode running to blocked
					printf("At time %d: Thread %d of Process %d moves from running to blocked\n", sim_time + thread.b_list[thread.cur_b].cpu_left, thread.thread_num + 1, thread.parent_process + 1);
				}

				thread.arrive = thread.b_list[thread.cur_b].cpu_left + thread.b_list[thread.cur_b].io + sim_time;
				int cpu_l = sim->proc_list[thread.parent_process].t_list[thread.thread_num].b_list[thread.cur_b].cpu_left;
				sim->proc_list[thread.parent_process].t_list[thread.thread_num].cpu_tot += thread.b_list[thread.cur_b].cpu;
				next_e = thread.b_list[thread.cur_b].cpu_left;
				thread.cur_b++;
			}

			arr_time = next_e;
			sim_time += arr_time;

			//if-else below deals with if the thread is finished else go to next burst
			if (thread.cur_b >= thread.cpu_bursts) {//This is the last burst and does
				// not go back into the queue
				
				//Proper array access
				thread.cur_b--;

				if (thread.b_list[thread.cur_b].cpu_left <= quantum) {//There is no cpu service time left
					//terminate this thread
					threadTerminated++;
					sim->proc_list[thread.parent_process].t_list[thread.thread_num].time_finished = sim_time;
					thread.time_finished = sim_time;
					sim->proc_list[thread.parent_process].threads_terminated++;

					//Process is finished
					if (sim->proc_list[thread.parent_process].threads_terminated == sim->proc_list[thread.parent_process].tnum) {
						printf("This process is complete at time %d\n", sim_time);
						sim->proc_list[thread.parent_process].time_finished = sim_time;
					}

					if (flags[1] == 1) {//Verbose mode
						thread.cpu_tot = sim->proc_list[thread.parent_process].t_list[thread.thread_num].cpu_tot;
						for (int i = 0; i < sim->proc_list[thread.parent_process].t_list[thread.thread_num].cpu_bursts - 1; i++) {
							thread.io_tot += sim->proc_list[thread.parent_process].t_list[thread.thread_num].b_list[i].io;
						}
						printf("At time %d: Thread %d of Process %d moves from running to terminated\n", sim_time, thread.thread_num + 1, thread.parent_process + 1);
						//Give summary info
						printf("Arrival time: %d\n", thread.init_arrive);
						printf("Service time: %d units, IO time: %d units, turnaround time: %d units, finish time: %d\n"
						, thread.cpu_tot, thread.io_tot, (thread.time_finished - thread.init_arrive), thread.time_finished);
						printf("\n");
					}	
				} else {//This is the last burst but there is some cpu service time left
					insert(pq, thread);		
				}
			} else {//Theres still some cpu bursts left and it needs to go back in queue
				insert(pq, thread);	
			}
			num_of_bursts++;
		} 
	}

	//Setup detailed mode
	float avg_turn_time = 0;
	int cpu_ut = 0;
	for (int i = 0; i < sim->process; i++) {
		avg_turn_time += sim->proc_list[i].time_finished;
		for (int j = 0; j < sim->proc_list[i].tnum; j++) {
			cpu_ut += sim->proc_list[i].t_list[j].cpu_tot;
		}
	}
	avg_turn_time = avg_turn_time/sim->process;
	float utilization = (float)cpu_ut / (float)sim_time * 100;

	//Defult print statements
	printf("\nTotal time required = %d\n", sim_time);
	printf("Average turnaround time = %.2f\n", avg_turn_time);
	printf("CPU Utilization is %.1f%%\n\n\n", utilization);

	//Detailed flag given
	if (flags[0] == 1) {
		for (int i = 0; i < sim->process; i++) {
			for (int j = 0; j < sim->proc_list[i].tnum; j++) {
				t_type t = sim->proc_list[i].t_list[j];
				t.io_tot = 0;
				for (int k = 0; k < sim->proc_list[i].t_list[j].cpu_bursts - 1; k++) {
					t.io_tot += sim->proc_list[i].t_list[j].b_list[k].io;
				}
				printf("Thread %d of Process %d: \n", j+1, i+1);
				printf("Arrival time: %d\n", t.init_arrive);
				printf("Service time: %d units, IO time: %d units, turnaround time: %d units, finish time: %d\n"
				, t.cpu_tot, t.io_tot, (t.time_finished - t.init_arrive), t.time_finished);
				printf("\n");
			}
		}
	}
	free(pq->arr);
	free(pq);
	free(line);
	free(backupLine);
	free_mem(sim);
}
