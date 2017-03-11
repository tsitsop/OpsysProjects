/*Zachary Maurer, George Tsitsopoulos, Jose Guvenilir*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "algorithms.h"

int parseFile(processInfo*, char*);

int main(int argc, char* argv[]) {
	if (argc != 3) {
		fprintf(stderr, "ERROR: Invalid Arguments\n");
		fprintf(stderr, "USAGE: ./a.out <input-file> <stats-output-file>\n");

		return EXIT_FAILURE;
	}

	/* Create array to hold information from input file */
	processInfo* inputInfo = malloc(26 * sizeof(processInfo));

	/* Parse the input file, populating inputInformation */
	
	const int numProcesses = parseFile(inputInfo, argv[1]);

	#ifdef DEBUG_MODE
		int i = 0;

		for(i;i<numProcesses;i++) {
			printf("PID: %c\n", inputInfo[i].processID);
			printf("Arrival Time: %d\n", inputInfo[i].arrivalTime);
			printf("CPU Burst Time: %d\n", inputInfo[i].cpuBurstTime);
			printf("Number of CPU bursts: %d\n", inputInfo[i].numBursts);
			printf("IO Time: %d\n\n", inputInfo[i].ioTime);
		}
	#endif

	/* Simulate with First Come First Serve */
	fcfs(inputInfo, numProcesses, argv[2]);
	//printf("process %c numBursts = %d\n", inputInfo[0].processID, inputInfo[0].numBursts);

	parseFile(inputInfo, argv[1]);
	srt(inputInfo, numProcesses, argv[2]);

	parseFile(inputInfo, argv[1]);
	rr(inputInfo, numProcesses, argv[2]);

	// printf("hello world we made it\n");
	free(inputInfo);

	return 0;
}

int parseFile(processInfo* inputInfo, char* fileName) {
	/* Open File */
	FILE* fp = fopen(fileName, "r");
	if (fp == NULL) {
		fprintf(stderr, "ERROR: Could not open %s\n", fileName);
		
		exit(1);
	}

	int i = 0; /* Number of processes stored */
	char c;
	char buffer[256];


	/* Parse file */
	while (!feof(fp)) { 
		c = fgetc(fp);

		if (feof(fp)) {
			break;
		}

		if (c == '#') { /* Skip this line because it is a comment */
			fgets(buffer, sizeof(buffer), fp);
		} else if (isspace(c)) { /* Skip this line because it is whitespace */
			fgets(buffer, sizeof(buffer), fp);
		} else { /* Read in the process information */
			/* Rewind the file pointer by one */
			fseek(fp, -1, SEEK_CUR);

			fscanf(fp, "%c|%d|%d|%d|%d\n", &inputInfo[i].processID,
				&inputInfo[i].arrivalTime, &inputInfo[i].cpuBurstTime,
				&inputInfo[i].numBursts, &inputInfo[i].ioTime);

			inputInfo[i].ioTimeRemaining = -1;
			inputInfo[i].totalWaitTime = 0;
			inputInfo[i].totalTurnaroundTime = 0;
			inputInfo[i].timeRemaining = inputInfo[i].cpuBurstTime;
			
			#ifdef DEBUG_MODE
				printf("PID: %c\n", inputInfo[i].processID);
				printf("Arrival Time: %d\n", inputInfo[i].arrivalTime);
				printf("CPU Burst Time: %d\n", inputInfo[i].cpuBurstTime);
				printf("Number of CPU bursts: %d\n", inputInfo[i].numBursts);
				printf("IO Time: %d\n\n", inputInfo[i].ioTime);
			#endif

			i += 1;
		}
	}

	/* Close file */
	if (fclose(fp) != 0) {
		fprintf(stderr, "ERROR: Could not close %s\n", fileName);

		exit(1);
	}

	return i;
}