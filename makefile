CC = gcc
CFLAGS = -std=gnu99 -Wpedantic

all: simcpu

simcpu.o: simcpu.c
	$(CC) $(CFLAGS) -c simcpu.c -o simcpu.o

simcpu: simcpu.o simcpu_func.o min_heap.o
	$(CC) $(CFLAGS) simcpu.o simcpu_func.o min_heap.o -o simcpu

simcpu_func.o: simcpu_func.c
	$(CC) $(CFLAGS) -c simcpu_func.c -o simcpu_func.o

min_heap.o: min_heap.c
	$(CC) $(CFLAGS) -c min_heap.c -o min_heap.o

min_heap: min_heap.o
	$(CC) $(CFLAGS) min_heap.o -o min_heap

clean:
	rm *.o simcpu

