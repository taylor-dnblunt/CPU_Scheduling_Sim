typedef struct burst {
	int cpu;
	int io;
} burst;

typedef struct threads {
	int arrive;
	int cpu_bursts;
	int cur_b;
	burst * b_list;
} t_type;

typedef struct process {
	int tnum;
	int cur_tnum;
	t_type * array;
} proc;

typedef struct sim_cont {
	int process;
	int cur_proc;
	proc * proc_list;
	int same_switch;
	int dif_switch;
} sim_cont;

void flag_checker(int argc, char * argv[], int * flag_arr);
void line_parse(char * line, int * num_arr);
