typedef struct threads {
	int arrive;
	int cpu_bursts;
} t_type;

typedef struct process {
	int tnum;
	t_type * head;
} proc;

typedef struct sim_cont {
	int process;
	int same_switch;
	int dif_switch;
} sim_cont;

void flag_checker(int argc, char * argv[], int * flag_arr);
void line_parse(char * line, int * num_arr);
