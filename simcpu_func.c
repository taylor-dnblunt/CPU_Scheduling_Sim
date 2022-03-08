#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void flag_checker(int argc, char * argv[], int * flag_arr) {

	printf("FLAG_CHECKER\n");

	for (int i = 1; i < argc; i++) {
		if (strcmp("-d", argv[i]) == 0) {
			printf("set detailed mode\n");
			flag_arr[0] = 1;
		}
		if (strcmp("-v", argv[i]) == 0) {
			printf("set verbose mode\n");
			flag_arr[1] = 1;
		}
		if (strcmp("-r", argv[i]) == 0) {
			printf("Round robin schedling with time quantum = ");
			flag_arr[2] = 1;
			printf("%d\n", atoi(argv[argc - 1]));
		}
	}
}

void line_parse() {
}
