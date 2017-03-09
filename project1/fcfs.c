#include <stdio.h>
#include "algorithms.h"

void fcfs(processInfo* processes, const int n, const char* outputFileName) {
	const int m = 1; /* number of processors */
	const int t_cs = 6; /* context switch time */

	int z = 0;

	/*for(z;z<n;z++) {
		printf("PID: %c\n", processes[z].processID);
		printf("Arrival Time: %d\n", processes[z].arrivalTime);
		printf("CPU Burst Time: %d\n", processes[z].cpuBurstTime);
		printf("Number of CPU bursts: %d\n", processes[z].numBursts);
		printf("IO Time: %d\n\n", processes[z].ioTime);
	}*/

	int i = 0, t = 0;
	int inCPUBurst = 0, inIOBurst = 0;
	myQueue readyQueue;
	int burstTimes[n], waitTimes[n], turnaroundTimes[n]; 
	algorithmStats stats;
	createQueue(&readyQueue);
	processInfo* currentCPUProcess;
	int cpuTimeLeft = -1; //nothing using cpu
	int timeToBring = 0, timeToRemove = 0; //time to bring/remove process to/from cpu
	int numContextSwitches = 0, numPreemptions = 0;
	float avgBurstTime = 0;
	int numBursts=0;

	for(i = 0; i < n; i++) {
		avgBurstTime += (float)(processes[i].cpuBurstTime * processes[i].numBursts);
		numBursts += processes[i].numBursts;
	}
	avgBurstTime /= numBursts;


	while (1) {
		if (isEmpty(readyQueue)) {
			timeToBring = t_cs / 2;
		}

		/* If it is time for a process to arrive, add process to queue */
		for (i = 0; i < n; i++) {
			if (t == processes[i].arrivalTime) { 
				//printf("gonna insert process %c\n", processes[i].processID);
				//printf("%s\n\n",getQueue(readyQueue) );
	
				insert(&readyQueue, &(processes[i]));
				printf("time %dms: Process %c arrived and added to ready queue %s\n", t, processes[i].processID, getQueue(readyQueue));
			}
		}

		if (cpuTimeLeft == -1 && timeToBring == 0 && timeToRemove == 0) { /* If nothing is using the cpu and context switch complete then add a process from the queue */
			if (!isEmpty(readyQueue)) {
				currentCPUProcess = pop(&readyQueue);
				cpuTimeLeft = currentCPUProcess->cpuBurstTime;
				printf("time %dms: Process %c started using the CPU %s\n", t, currentCPUProcess->processID, getQueue(readyQueue));
			}
		} else if (cpuTimeLeft == 0) { /* if a process finishes using the cpu */
			currentCPUProcess->numBursts -= 1;
			if (currentCPUProcess->numBursts != 0) {
				printf("time %dms: Process %c completed a CPU burst; %d bursts to go %s\n",t, currentCPUProcess->processID, currentCPUProcess->numBursts, getQueue(readyQueue));
				
				currentCPUProcess->ioTimeRemaining = currentCPUProcess->ioTime + t_cs/2; //currentCPUProcess will be in IO for its IO time + the time it takes to remove from cpu
				printf("time %dms: Process %c switching out of CPU; will block on I/O until time %dms %s\n", t, currentCPUProcess->processID, t + currentCPUProcess->ioTimeRemaining, getQueue(readyQueue));
			} else {
				printf("time %dms: Process %c terminated %s\n", t, currentCPUProcess->processID, getQueue(readyQueue));
			}

			numContextSwitches += 1;
			cpuTimeLeft = -1;
			timeToBring = t_cs/2; //reset the time until you can 
			timeToRemove = t_cs/2;
		}

		/* IO Burst maintenence */
		for (i = 0; i < n; i++) {
			if (processes[i].ioTimeRemaining == -1) { //if process not in io do nothing
				continue;
			} else if (processes[i].ioTimeRemaining == 0) { //if process completes io then insert to readyQueue
				processes[i].ioTimeRemaining = -1;
				insert(&readyQueue, &(processes[i]));
				printf("time %dms: Process %c completed I/O; added to ready queue %s\n", t, processes[i].processID, getQueue(readyQueue));
			} else { // if process is in io but not complete, decrement the counter
				processes[i].ioTimeRemaining -= 1;
			}
		}

		//if nothing in cpu and readyQueue is empty
		if (cpuTimeLeft == -1 && isEmpty(readyQueue)) {
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

		/* decrement cpu timer */
		if (cpuTimeLeft != -1) { // if the cpu is currently being used
			cpuTimeLeft -= 1; // decrement timer
		} else { // if cpu is not being used, context switch stuff
			if (timeToRemove == 0 && timeToBring > 0) { // if have removed a process from the cpu and are waiting to add one
				timeToBring -= 1;
			} else if (timeToRemove > 0) { // if you still haven't removed a process from the cpu, count down
				timeToRemove -= 1;
			}
		}

		for (i = 0; i < n; i++) {
			// if (processes[i].arrivalTime < t) { //if the process has arrived
			// 	if (processes[i].numBursts != 0) { //and the process isnt completed yet
			// 		if (processes[i].processID != currentCPUProcess->processID) { //and the process is not using the cpu
			// 			if (processes[i].ioTimeRemaining == -1) { //if its not in IO
			// 				if (timeToBring == 0 && timeToRemove == 0) {
			// 					processes[i].totalWaitTime += 1;
			// 				}
			// 			}
			// 		} 
			// 	}
			// }
			if (contains(readyQueue, processes[i].processID)) {
				if (timeToBring == 0 && timeToRemove == 0) {
					processes[i].totalWaitTime += 1;
				}
			}
		}



		//increment timer
		t += 1;
	}


	float avgWaitTime = 0, avgTurnaroundTime = 0;

	for (i = 0; i < n; i++) {
		avgWaitTime += (float) processes[i].totalWaitTime;
		// avgTurnaroundTime += (float) (processes[i].cpuBurstTime + processes[i].totalWaitTime);
	}
	avgWaitTime /= (float)n;
	avgTurnaroundTime = avgBurstTime+avgWaitTime;

	printf("Algorithm FCFS\n");
	printf("-- average CPU burst time: %f ms\n", avgBurstTime );
	printf("-- average wait time: %f\n", avgWaitTime );
	printf("-- average turnaround time: %f\n", avgTurnaroundTime);
	printf("-- total number of context switches: %d\n", numContextSwitches );
	printf("-- total number of preemptions: %d\n", numPreemptions );








	freeQueue(&readyQueue);
	return;
}



/* 

	--need to figure out average stuff
	-- probably test before??


*/