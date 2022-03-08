CC = gcc
CFLAGS = -std=gnu99 -pthread -Wpedantic

all: simcpu

simcpu.o: simcpu.c
	$(CC) $(CFLAGS) -c simcpu.c -o simcpu.o

simcpu: simcpu.o simcpu_func.o
	$(CC) $(CFLAGS) simcpu.o simcpu_func.o -o simcpu

simcpu_func.o: simcpu_func.c
	$(CC) $(CFLAGS) -c simcpu_func.c -o simcpu_func.o

clean:
	rm *.o simcpu

