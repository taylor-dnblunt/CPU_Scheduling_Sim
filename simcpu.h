
typedef struct node {
	struct node * left;
	struct node * right;
	int value;
} node;

typedef struct sim_cont {
	int process;
	int same_switch;
	int dif_switch;
} sim_cont;

void flag_checker(int argc, char * argv[], int * flag_arr);
