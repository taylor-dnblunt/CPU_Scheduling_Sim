CC = gcc
CFLAGS = -std=gnu99 -pthread -Wpedantic

all: simcpu

simcpu.o: simcpu.c
	$(CC) $(CFLAGS) -c simcpu.c -o simcpu.o

simcpu: simcpu.o simcpu_func.o heap.o
	$(CC) $(CFLAGS) simcpu.o simcpu_func.o heap.o -o simcpu

simcpu_func.o: simcpu_func.c
	$(CC) $(CFLAGS) -c simcpu_func.c -o simcpu_func.o

heap.o: heap.c
	$(CC) $(CFLAGS) -c heap.c -o heap.o

clean:
	rm *.o simcpu

