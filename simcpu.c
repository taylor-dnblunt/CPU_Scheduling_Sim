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
			//printf("thread 1 arrives %d and has %d cpu bursts\n", sim->proc_list[0].t_list[0].arrive, 
			//sim->proc_list[0].t_list[0].cpu_bursts);

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
				//printf("This line is the next thread in a process: ");
				//printf("%sand has %d bursts\n", backupLine, numBursts);

			} else if (threadCnt == numThreads) {
				//NEW PROCESS
				set_new_proc(sim, nums);

				//printf("This line is a process: ");
				//printf("%s", backupLine);
				numThreads = nums[1];
				threadFlag = 1;
				burstFlag = 0;
				//printf("num threads in this process = %d\n", numThreads);
			}

		} else if (burstFlag == 1) {
			//NEW BURST
			burstCnt++;
			//printf("This line is a burst: %s", backupLine);
			set_new_burst(sim, nums);

		} else if (threadFlag == 1) { //Next line is new thread info
			//printf("This line is a new thread after a process line: ");
			//printf("%s", backupLine);
			numBursts = nums[2];

			set_tAfterPLine(sim, nums);

			//printf("numBursts = %d and burstCnt = %d\n", numBursts, burstCnt);
			burstFlag = 1;
			threadCnt = 1;
		}
	}
	printf("\n\n\n");
	// printf("Line count = %d\n", lineCnt);
	// printf("p = %d, ss = %d, ds = %d\n", sim->process, sim->same_switch, sim->dif_switch);
	// printf("process 1 in the sim has %d threads and sim current process = %d\n",
	//  sim->proc_list[0].tnum, sim->cur_proc);
	// printf("second thread in process 1 has %d bursts\n", sim->proc_list[0].t_list[1].cpu_bursts);
	// printf("Parent process of this thread is %d\n", sim->proc_list[1].t_list[1].parent_process);
	// printf("Parent process of this thread is %d\n", sim->proc_list[0].t_list[3].parent_process);
	//printf("first thread in process 2 has %d bursts and arrived at %d\n",
	 //sim->proc_list[1].t_list[0].cpu_bursts, sim->proc_list[1].t_list[0].arrive);
	
	
	//Start making queue
	int capacity = sim->process * 50;
	printf("Capacity of heap = %d\n", capacity);
	int heap_type = 0;//for min heap

	//Initialize pq
	Heap * pq = CreateHeap(capacity, heap_type);


	//Keep running while all threads or processes are not terminated
	int threadTerminated = 0;
	int threadTot = 0;
	//printf("last burst has num %d and total burst count is %d", sim->proc_list[1].t_list[0].b_list[0].num, sim->proc_list[1].t_list[0].cpu_bursts);
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
	for (int i = 0; i < sim->process;i++) {
		threadTot += sim->proc_list[i].tnum;
		for(int j = 0; j < sim->proc_list[i].tnum;j++) {
			//This makes a new thread that has the same properties but isnt referencing the list
			t_type thread = sim->proc_list[i].t_list[j];
			//Reset cur_b so it starts on the first burst and not the last
			thread.cur_b = 0;
			printf("thread arrival = %d\n", thread.arrive);
			insert(pq, thread);
		}
	}
	printf("Thread total = %d\n\n", threadTot);
	t_type thread;
	int arr_time = 0;
	int next_e = 0;
	int num_of_bursts = 0;
	int prev_proc_num = -1;
	int prev_t;
	int prev_b;
	//Need a total time to keep track of everything
	int sim_time = 0;
	printf("numbursts in process 2 thread 1 = %d\n", sim->proc_list[1].t_list[0].cpu_bursts);
	printf("The cpu in P1 T2 B2 = %d\n\n\n", sim->proc_list[0].t_list[1].b_list[1].cpu_left);


	//Essentially while pq not empty i.e there are still threads that need processing
	while (threadTerminated < threadTot) {

		//This is the start of FCFS
		if (flags[2] != 1) {//1 being flag -r was present and a time quantum given for rr
			//printf("FCFS\n");
			
			//Threads switch between cpu time and IO
			thread = PopMin(pq);

			//printf("This thread is P%d B%d with arrival thread arr time %d\n", thread.parent_process, thread.cur_b, thread.arrive);
			if (thread.arrive == thread.init_arrive && flags[1] == 1) {
				printf("At time %d: Thread %d of Process %d moves from new to ready\n", thread.init_arrive, thread.thread_num + 1, thread.parent_process + 1);
			} else if (thread.arrive != thread.init_arrive && flags[1] == 1) {//Blocked to ready
				printf("At time %d: Thread %d of Process %d moves from blocked to ready\n", thread.arrive, thread.thread_num + 1, thread.parent_process + 1);
			}

			if (prev_proc_num != -1) {//Check to make sure its not the first thread when theres no prev proc
				if (thread.parent_process == prev_proc_num) {//Same process switch time
					sim_time += sim->same_switch;
				} else if (thread.parent_process != prev_proc_num) {//Dif process switch time
					sim_time += sim->dif_switch;
				}
			}

			//printf("Enters cpu at %d\n", sim_time);
			if (flags[1] == 1) {
				printf("At time %d: Thread %d of Process %d moves from ready to running\n", sim_time, thread.thread_num + 1, thread.parent_process + 1);				
			}
			//Set when to enter the ready q again
			thread.arrive = thread.b_list[thread.cur_b].cpu + thread.b_list[thread.cur_b].io + sim_time;

			next_e = thread.b_list[thread.cur_b].cpu;
			if (flags[1] == 1) {//Verbose mode running to blocked
					printf("At time %d: Thread %d of Process %d moves from running to blocked\n", sim_time + next_e, thread.thread_num + 1, thread.parent_process + 1);
			}
			arr_time = next_e;
			sim_time += arr_time;
			prev_proc_num = thread.parent_process;
			//cpu time is right but IO is wrong
			sim->proc_list[thread.parent_process].t_list[thread.thread_num].cpu_tot += thread.b_list[thread.cur_b].cpu;
			
			//Below goes to the next burst not the current one
			thread.cur_b++;

			if (thread.cur_b == thread.cpu_bursts) {//This is the last burst and does
				// not go back into the queue
				threadTerminated++;
				sim->proc_list[thread.parent_process].t_list[thread.thread_num].time_finished = sim_time;
				thread.time_finished = sim_time;
				//Count up threads terminated for the process and store time once both are done
				sim->proc_list[thread.parent_process].threads_terminated++;
				if (sim->proc_list[thread.parent_process].threads_terminated == sim->proc_list[thread.parent_process].tnum) {
					//printf("This process is complete at time %d\n", sim_time);
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
			} else {//Theres still some cpu bursts left and it needs to go back in queue
				insert(pq, thread);	
			}
			//Thread goes out for io after cpu and when back needs new burst
			
			
			//printf("New event time after cpu burst %d\n", sim_time);
			//if statement for at event time if there are no threads that have arrived
			//go to next thread event time
			num_of_bursts++;
			

		//This is the start of round robin

		} else if (flags[2] == 1) {//Flag -r and quantum are given
			//printf("RR\n");

			//Threads switch between cpu time and IO
			thread = PopMin(pq);
			if (thread.arrive == thread.init_arrive && flags[1] == 1) {
				printf("At time %d: Thread %d of Process %d moves from new to ready\n", thread.init_arrive, thread.thread_num + 1, thread.parent_process + 1);
			} else if (thread.arrive != thread.init_arrive && flags[1] == 1) {//Blocked to ready
				printf("At time %d: Thread %d of Process %d moves from blocked to ready\n", thread.arrive, thread.thread_num + 1, thread.parent_process + 1);
			}
			//printf("This thread is P%d T%d B%d with arrival time %d\n", thread.parent_process+1, thread.thread_num+1, thread.cur_b+1, thread.arrive);
			
			if (prev_proc_num != -1) {//Check to make sure its not the first thread when theres no prev proc
				if (thread.parent_process == prev_proc_num && thread.thread_num == prev_t && thread.cur_b == prev_b) {
					//This is the same burst on the same thread
					//printf("This is the same burst\n");
				} else {
					if (thread.parent_process == prev_proc_num) {//Same process switch time
						sim_time += sim->same_switch;
					} else if (thread.parent_process != prev_proc_num) {//Dif process switch time
						sim_time += sim->dif_switch;
					}
				}
			}
			if (thread.arrive > sim_time) {
				sim_time = thread.arrive;
			}
			//Setup for next burst arrival
			prev_proc_num = thread.parent_process;
			prev_t = thread.thread_num;
			prev_b = thread.cur_b;

			//printf("Enters cpu at %d\n", sim_time);
			if (flags[1] == 1) {
				printf("At time %d: Thread %d of Process %d moves from ready to running\n", sim_time, thread.thread_num + 1, thread.parent_process + 1);
			}


			//If not done current cpu service stay and decrement time left
			if (thread.b_list[thread.cur_b].cpu_left > quantum) {//Subtract quanta time from service time 

				if (flags[1] == 1) {//Verbose mode running to blocked
					printf("At time %d: Thread %d of Process %d moves from running to blocked\n", sim_time + quantum, thread.thread_num + 1, thread.parent_process + 1);
				}
				thread.arrive = quantum + thread.b_list[thread.cur_b].io + sim_time;
				//printf("re-entry = sim_time %d + quantum %d + io %d\n", sim_time, quantum, thread.b_list[thread.cur_b].io);
				sim->proc_list[thread.parent_process].t_list[thread.thread_num].b_list[thread.cur_b].cpu_left -= quantum;
				//thread.b_list[thread.cur_b].cpu_left -= quantum;
				//printf("This burst still has service time left %d\n", thread.b_list[thread.cur_b].cpu_left);
				next_e = quantum;//Supposed to pop ahead proper amount to next event
				// if(thread.b_list[thread.cur_b].cpu_left <= 0) {//Probably dont need but keep for now
				// 	thread.cur_b++;
				// }
			//Else done current burst cpu service go to next in thread
			} else if (thread.b_list[thread.cur_b].cpu_left <= quantum) {//Only go service time before io
				
				if (flags[1] == 1) {//Verbose mode running to blocked
					printf("At time %d: Thread %d of Process %d moves from running to blocked\n", sim_time + thread.b_list[thread.cur_b].cpu_left, thread.thread_num + 1, thread.parent_process + 1);
				}
				thread.arrive = thread.b_list[thread.cur_b].cpu_left + thread.b_list[thread.cur_b].io + sim_time;
				int cpu_l = sim->proc_list[thread.parent_process].t_list[thread.thread_num].b_list[thread.cur_b].cpu_left;
				//printf("re-entry = sim_time %d + cpu %d + io %d\n", sim_time, cpu_l, thread.b_list[thread.cur_b].io);
				sim->proc_list[thread.parent_process].t_list[thread.thread_num].cpu_tot += thread.b_list[thread.cur_b].cpu;
				//Below goes to the next burst not the current one
				next_e = thread.b_list[thread.cur_b].cpu_left;//Check next_e's they may be wrong
				thread.cur_b++;
				//This line below may be the problem causer
				// thread.b_list[thread.cur_b].cpu_left = 0;
				//printf("On burst %d of bursts %d\n", thread.cur_b, thread.cpu_bursts);
				if (thread.cur_b == thread.cpu_bursts) {
					printf("Thread is finished\n");
				} else {
					printf("Next burst\n");
				}
				
			}
			arr_time = next_e;
			sim_time += arr_time;

			//fi-else below deals with if the thread is finished else go to next burst
			if (thread.cur_b >= thread.cpu_bursts) {//This is the last burst and does
				// not go back into the queue
				//Had to -- cur_b so the array was accessed properly
				thread.cur_b--;

				//printf("cpu time left %d\n", thread.b_list[thread.cur_b].cpu_left);
				if (thread.b_list[thread.cur_b].cpu_left <= quantum) {//There is no cpu service time left
					//terminate this thread
					threadTerminated++;
					sim->proc_list[thread.parent_process].t_list[thread.thread_num].time_finished = sim_time;
					thread.time_finished = sim_time;
					//Count up threads terminated for the process and store time once both are done
					sim->proc_list[thread.parent_process].threads_terminated++;
					if (sim->proc_list[thread.parent_process].threads_terminated == sim->proc_list[thread.parent_process].tnum) {
						printf("This process is complete at time %d\n", sim_time);
						sim->proc_list[thread.parent_process].time_finished = sim_time;
					}
					printf("Thread termination time %d\n\n", sim_time);
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
					//printf("Thread re-enters the ready queue at time %d\n", thread.arrive);
					insert(pq, thread);		
				}
			} else {//Theres still some cpu bursts left and it needs to go back in queue
				//printf("Thread re-enters the ready queue at time %d\n", thread.arrive);
				insert(pq, thread);	
			}
			//go to next thread event time
			num_of_bursts++;
		} 
	}

	float avg_turn_time = 0;
	int cpu_ut = 0;
	for (int i = 0; i < sim->process; i++) {
		avg_turn_time += sim->proc_list[i].time_finished;
		for (int j = 0; j < sim->proc_list[i].tnum; j++) {
			cpu_ut += sim->proc_list[i].t_list[j].cpu_tot;
		}
	}
	avg_turn_time = avg_turn_time/sim->process;
	printf("cpu ut = %d\n", cpu_ut);
	float utilization = (float)cpu_ut / (float)sim_time * 100;

	printf("Total time required = %d\n", sim_time);
	printf("Average turnaround time = %.2f\n", avg_turn_time);
	// printf("There were %d cpu bursts and there should be 8\n", num_of_bursts);

	//CPU utilization = ((total - time in cpu)/total) * 100
	printf("CPU Utilization is %.1f%%\n\n\n", utilization);

	//Essentially the detailed flag
	if (flags[0] == 1) {//The -d flag is given
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
	

	free_mem(sim);
}
