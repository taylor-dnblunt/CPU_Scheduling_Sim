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

//Take line and return numbers from line in the num_arr
void line_parse(char * line, int * num_arr) {

	const char s[2] = " ";
	int i = 0;
	for (char * p = strtok(line, s); p != NULL; p = strtok(NULL, s)) {
		//printf("%d ", atoi(p));
		num_arr[i] = atoi(p);
		i++;
	}
	printf("line_parse\n");
}
