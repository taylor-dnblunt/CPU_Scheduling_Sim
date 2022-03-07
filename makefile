CC = gcc
CFLAGS = -std=gnu99 -pthread -Wpedantic

all: sched

sched.o: sched.c
	$(CC) $(CFLAGS) -c sched.c -o sched.o

sched: sched.o
	$(CC) $(CFLAGS) sched.o -o sched

clean:
	rm *.o sched

