typedef struct burst {
	int num;
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
    int *arr;
    int count;
    int capacity;
    int heap_type; // for min heap , 1 for max heap
};
typedef struct Heap Heap;

/*void flag_checker(int argc, char * argv[], int * flag_arr);
void line_parse(char * line, int * num_arr);
void free_mem(sim_cont * sim);
void swap(int *a, int *b);
void heapify(int array[], int size, int i);
void insert(int array[], int newNum);
void deleteRoot(int array[], int num);
void printArray(int array[], int size);*/


Heap *CreateHeap(int capacity,int heap_type);
void insert(Heap *h, int key);
void print(Heap *h);
void heapify_bottom_top(Heap *h,int index);
void heapify_top_bottom(Heap *h, int parent_node);
int PopMin(Heap *h);