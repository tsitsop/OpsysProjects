#include <stdio.h>
#include "algorithms.h"

void fcfs(processInfo (*inputInfo)[26], const int n, const char* outputFileName) {
	//const int m = 1; /* number of processors */
	//const int t_cs = 6; /* context switch time */
	
	#ifdef DEBUG_MODE
		printf("PID: %c\n", inputInfo[0]->processID);
		printf("Arrival Time: %d\n", inputInfo[0]->arrivalTime);
		printf("CPU Burst Time: %d\n", inputInfo[0]->cpuBurstTime);
		printf("Number of CPU bursts: %d\n", inputInfo[0]->numBursts);
		printf("IO Time: %d\n\n", inputInfo[0]->ioTime);
	#endif



	int i = 0;;
	myQueue q;
	createQueue(&q);

	for (i=0;i<n;i++) {
		insert(&q, *inputInfo[i]);
		printf("PID: %c\n", inputInfo[i]->processID);
	}

	freeQueue(&q);


	/* 
		1. start timer
		2. loop through processes and add the ones with earliest arrival time to queue
		3. start first process on cpu
		4. run until burst complete
			1. upon completion, change num bursts in the inputInfo
			2. add to queue
	*/
}