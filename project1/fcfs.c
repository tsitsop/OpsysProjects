#include <stdio.h>
#include "algorithms.h"

void fcfs(processInfo* inputInfo, const int n, const char* outputFileName) {
	//const int m = 1; /* number of processors */
	//const int t_cs = 6; /* context switch time */

	int i = 0;
	myQueue q;
	createQueue(&q);

	// Testing insert
	for (i=0;i<n;i++) {
		insert(&q, inputInfo[i]);
		printf("PID: %c\n", inputInfo[i].processID);
		printf("Arrival Time: %d\n", inputInfo[i].arrivalTime);
		printf("CPU Burst Time: %d\n", inputInfo[i].cpuBurstTime);
		printf("Number of CPU bursts: %d\n", inputInfo[i].numBursts);
		printf("IO Time: %d\n\n", inputInfo[i].ioTime);
	}

	//Testing pop
	for (i=0;i<n;i++) {
		printf("%c\n", pop(&q));
	}

	freeQueue(&q);
}