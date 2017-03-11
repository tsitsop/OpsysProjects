#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "algorithms.h"

void fcfs(processInfo* processes, const int n, const char* outputFileName) {
	// const int m = 1; /* number of processors */
	const int t_cs = 6; /* context switch time */
	const int t_slice = 94; /* time slice length */

	int i = 0, t = 0;
	int inIOBurst = 0;
	myQueue readyQueue;
	createQueue(&readyQueue);
	processInfo* currentCPUProcess;
	int timeToBring = 0, timeToRemove = 0; //time to bring/remove process to/from cpu
	int numContextSwitches = 0, numPreemptions = 0;
	float avgBurstTime = 0;
	int numBursts=0;
	int entering = 0; // is something mid cpu entrance?
	char* qStr;
	int timeSliceRemaining = -1;

	for(i = 0; i < n; i++) {
		avgBurstTime += (float)(processes[i].cpuBurstTime * processes[i].numBursts);
		numBursts += processes[i].numBursts;
	}
	avgBurstTime /= numBursts;


	qStr = malloc(4 + 2*readyQueue.itemCount*sizeof(char)*sizeof(char));
	printf("time %dms: Simulator started for RR %s\n", t, getQueue(readyQueue, qStr));
	fflush(stdout);
	while (1) {
		//if nothing is in the readyQueue then it should constantly take timeToBring ms to bring the next process in
		if (isEmpty(readyQueue) && !entering) {
			timeToBring = t_cs / 2;
		}

		/* If it is time for a process to arrive, add process to queue */
		for (i = 0; i < n; i++) {
			if (t == processes[i].arrivalTime) {
				insert(&readyQueue, &(processes[i]));
				qStr = realloc(qStr, 4 + 2*readyQueue.itemCount*sizeof(char)*sizeof(char));
				printf("time %dms: Process %c arrived and added to ready queue %s\n", t, processes[i].processID, getQueue(readyQueue, qStr));
				fflush(stdout);
			}
		}

		/* IO Burst maintenence */
		for (i = 0; i < n; i++) {
			if (processes[i].ioTimeRemaining == -1) { //if process not in io do nothing
				continue;
			} else if (processes[i].ioTimeRemaining == 1) { //if process completes io then insert to readyQueue   ////////////////////////////////////////changed this to 1 from 0
				processes[i].ioTimeRemaining = -1;
				insert(&readyQueue, &(processes[i]));
				qStr = realloc(qStr, 4 + 2*readyQueue.itemCount*sizeof(char)*sizeof(char));
				printf("time %dms: Process %c completed I/O; added to ready queue %s\n", t, processes[i].processID, getQueue(readyQueue, qStr));
				fflush(stdout);
			} else { // if process is in io but not complete, decrement the counter
				processes[i].ioTimeRemaining -= 1;
			}
		}








		// if a process has been removed from the cpu and nothing is entering yet, 
		if (timeSliceRemaining == -1 && timeToRemove == 0 && entering == 0) { //process removed from cpu, nothing added yet
			if (!isEmpty(readyQueue)) { // as long as ready queue not empty
				// printf("something is gonna enter in 3ms...\n");
				entering = 1; //flag to show that a process is being brought into the cpu
				timeToBring = t_cs/2; // count down till you can bring it in
			}

		}

		// if a process was entering the cpu and is now in there, it can use the cpu
		if (entering == 1 && timeToBring == 0) {
			// printf("poppin bottles bb\n");
			currentCPUProcess = pop(&readyQueue); // pop whatever is in front
			currentCPUProcess->totalWaitTime -= t_cs/2; 
			timeSliceRemaining = t_slice; // reset the time slice counter
			

			qStr = realloc(qStr, 4 + 2*readyQueue.itemCount*sizeof(char)*sizeof(char));
			

			if (currentCPUProcess->timeRemaining == currentCPUProcess->cpuBurstTime) {
				printf("time %dms: Process %c started using the CPU %s\n", t, currentCPUProcess->processID, getQueue(readyQueue, qStr));
			} else {
				printf("time %dms: Process %c started using the CPU with %dms remaining %s\n", t, currentCPUProcess->processID, currentCPUProcess->timeRemaining, getQueue(readyQueue, qStr));
			}
			fflush(stdout);
			entering = 0;
		}

		//if a process completes its cpu burst, it must be removed from the cpu
		if (timeSliceRemaining != -1) {
			if (currentCPUProcess->timeRemaining == 0) {
				currentCPUProcess->numBursts -= 1;
				numContextSwitches += 1;
				timeSliceRemaining = -1;
				timeToRemove = t_cs/2;

				if (currentCPUProcess->numBursts > 0) {
					qStr = realloc(qStr, 4 + 2*readyQueue.itemCount*sizeof(char)*sizeof(char));
					printf("time %dms: Process %c completed a CPU burst; %d bursts to go %s\n",t, currentCPUProcess->processID, 
						currentCPUProcess->numBursts, getQueue(readyQueue, qStr));
					fflush(stdout);
					currentCPUProcess->ioTimeRemaining = currentCPUProcess->ioTime + timeToRemove; //////////////////////////////////////////////////////////////// undid subtracted 1
					qStr = realloc(qStr, 4 + 2*readyQueue.itemCount*sizeof(char)*sizeof(char));
					printf("time %dms: Process %c switching out of CPU; will block on I/O until time %dms %s\n", t, 
						currentCPUProcess->processID, t + currentCPUProcess->ioTimeRemaining, getQueue(readyQueue, qStr));
					fflush(stdout);

					currentCPUProcess->timeRemaining = currentCPUProcess->cpuBurstTime;
				} else {
					qStr = realloc(qStr, 4 + 2*readyQueue.itemCount*sizeof(char)*sizeof(char));
					printf("time %dms: Process %c terminated %s\n", t, currentCPUProcess->processID, getQueue(readyQueue, qStr));
					fflush(stdout);
				}
			}
		}


		//if the timeslice expires, perform context switch. this will only be entered if time slice expired but burst didn't complete.
		if (timeSliceRemaining == 0) {
			if (!isEmpty(readyQueue)) {
				qStr = realloc(qStr, 4 + 2*readyQueue.itemCount*sizeof(char)*sizeof(char));
				printf("time %dms: Time slice expired; process %c preempted with %dms to go %s\n", t, currentCPUProcess->processID, currentCPUProcess->timeRemaining, getQueue(readyQueue, qStr));
				insert(&readyQueue, currentCPUProcess);
				numPreemptions += 1;
				numContextSwitches += 1;
				timeSliceRemaining = -1;
			} else {
				qStr = realloc(qStr, 4 + 2*readyQueue.itemCount*sizeof(char)*sizeof(char));
				printf("time %dms: Time slice expired; no preemption because ready queue is empty %s\n", t, getQueue(readyQueue, qStr));
				timeSliceRemaining = t_slice;
			}
			timeToRemove = t_cs/2;
		}

		/* decrement context switch and cpu timers */
		if (timeSliceRemaining != -1) { // if the cpu is currently being used
			timeSliceRemaining -= 1; // decrement timer
			currentCPUProcess->timeRemaining -= 1;
		} else { // if cpu is not being used, context switch stuff
			if (entering == 1) { // if something is entering the cpu, count down until it can enter
				timeToBring -= 1;
			} else if (timeToRemove > 0) { // if something is leaving the cpu, count down until it is gone
				timeToRemove -= 1;
			}
		}

		//figure out wait time
		for (i = 0; i < n; i++) {
			if (contains(readyQueue, processes[i].processID)) {
					processes[i].totalWaitTime += 1;
			}

			if (t >= processes[i].arrivalTime && processes[i].numBursts > 0 ) {
				processes[i].totalTurnaroundTime += 1;
			}
		}

		//increment timer
		t += 1;

		// check if done
		// if nothing in cpu and readyQueue is empty
		if (timeSliceRemaining == -1 && isEmpty(readyQueue) && entering == 0 && timeToRemove == 0) {
			//Check to see what is in IO
			inIOBurst = 0;
			for (i = 0; i < n; i++) {
				if (processes[i].ioTimeRemaining != -1) { //this means something is in io
					inIOBurst = 1;
					break;
				}
			}

			// if nothing in IO, have completed and can exit fcfs
			if (!inIOBurst) {
				break;
			}
		}

	}

	printf("time %dms: Simulator ended for RR\n", t);
	fflush(stdout);

	float avgWaitTime = 0, avgTurnaroundTime = 0;
	for (i = 0; i < n; i++) {
		avgWaitTime += (float) processes[i].totalWaitTime;
		avgTurnaroundTime += (float) processes[i].totalTurnaroundTime;
		printf("%d\n", processes[i].totalTurnaroundTime );
	}

	avgWaitTime /= (float)numBursts;
	avgTurnaroundTime /= (float)numBursts;


	/* Open File */
	FILE* fp = fopen(outputFileName, "w");
	if (fp == NULL) {
		fprintf(stderr, "ERROR: Could not open %s\n", outputFileName);
		
		exit(1);
	}

	fprintf(fp, "Algorithm RR\n");
	fprintf(fp, "-- average CPU burst time: %0.2f ms\n", avgBurstTime );
	fprintf(fp, "-- average wait time: %0.2f ms\n", avgWaitTime );
	fprintf(fp, "-- average turnaround time: %0.2f ms\n", avgTurnaroundTime);
	fprintf(fp, "-- total number of context switches: %d\n", numContextSwitches );
	fprintf(fp, "-- total number of preemptions: %d\n", numPreemptions );

	/* Close file */
	if (fclose(fp) != 0) {
		fprintf(stderr, "ERROR: Could not close %s\n", outputFileName);

		exit(1);
	}

	free(qStr);
	freeQueue(&readyQueue);
	return;
}



/* 

	process arrives and enters ready queue

	process added from ready queue to cpu
		- takes 3 ms enter cpu

	process begins using cpu
		- uses cpu until either timeslice expires or burst time ends

	process completes cpu burst
		- added to io
			- takes 3 ms to leave cpu
		- terminates
			- takes 3 ms to leave cpu

	timeslice expires
		- takes 3 ms to leave cpu
			- process removed 


	process added to io

	process completes io
		- added to ready queue




	--need to figure out average stuff
	-- probably test before??


*/