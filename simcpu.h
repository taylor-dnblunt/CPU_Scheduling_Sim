typedef struct burst {
	int num;
	int cpu;
	int io;
} burst;

typedef struct threads {
	int arrive;
	int cpu_bursts;
	int cur_b;
	int parent_process;
	burst * b_list;
} t_type;

typedef struct process {
	int tnum;
	int cur_tnum;
	t_type * t_list;
} proc;

typedef struct sim_cont {
	int process;
	int cur_proc;
	proc * proc_list;
	int same_switch;
	int dif_switch;
} sim_cont;

struct Heap{
    t_type *arr;
    int count;
    int capacity;
    int heap_type; // for min heap , 1 for max heap
};
typedef struct Heap Heap;

//My helper funcs
void flag_checker(int argc, char * argv[], int * flag_arr);
void line_parse(char * line, int * num_arr);
void free_mem(sim_cont * sim);
void set_proc(sim_cont * sim, int * nums);
void set_init_proc(sim_cont * sim, int * nums);
void set_init_thread(sim_cont * sim, int * nums);
void set_new_thread(sim_cont * sim, int * nums);
void set_new_proc(sim_cont * sim, int * nums);
void set_new_burst(sim_cont * sim, int * nums);
void set_tAfterPLine(sim_cont * sim, int * nums);

//Unused from heap.c
/*void swap(int *a, int *b);
void heapify(int array[], int size, int i);
void insert(int array[], int newNum);
void deleteRoot(int array[], int num);
void printArray(int array[], int size);*/

//min_heap.c funcs
Heap *CreateHeap(int capacity,int heap_type);
void insert(Heap *h, t_type thread);
void print(Heap *h);
void heapify_bottom_top(Heap *h,int index);
void heapify_top_bottom(Heap *h, int parent_node);
t_type PopMin(Heap *h);