/*Zachary Maurer, George Tsitsopoulos, Jose Guvenilir*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "algorithms.h"

void rr(processInfo* processes, const int n, const char* outputFileName) {
	const int t_cs = 6; /* context switch time */
  const int t_slice = 94;
	// int z = 0;

	int i = 0, t = 0;
	int inIOBurst = 0;
	myQueue readyQueue;
	createQueue(&readyQueue);
	processInfo* currentCPUProcess;
	int cpuTimeLeft = -1; //nothing using cpu
	int timeToBring = 0, timeToRemove = 0; //time to bring/remove process to/from cpu
	int numContextSwitches = 0, numPreemptions = 0;
	float avgBurstTime = 0;
	int numBursts=0;
	int entering = 0; // is something mid cpu entrance?
  int preempting = 0;
	char* qStr;
  int slice_time = 0;

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





    if (slice_time>0&& slice_time%t_slice==0) // time slice has completed
    {
      if (isEmpty(readyQueue)) //if ready queue is empty, we cant add anything to cpu. reset time leftin cpu to 0
      {
        printf("time %dms: Time slice expired; no preemption because ready queue is empty %s\n",t,getQueue(readyQueue, qStr));   
        fflush(stdout);
      }
      else
      {
          printf("time %dms: Time slice expired; process %c preempted with %dms to go %s\n",t,currentCPUProcess->processID,currentCPUProcess->timeRemaining,getQueue(readyQueue, qStr));   
     		fflush(stdout);
     	    numContextSwitches += 1;
			    cpuTimeLeft = -1;
			    timeToRemove = t_cs/2;
          slice_time = 0;
          preempting = 1;
          numPreemptions+=1;
      }
    }
    if (timeToRemove==0 && cpuTimeLeft == -1 && preempting == 1)
    {
      insert(&readyQueue,currentCPUProcess);
      
      preempting = 0;
    }
		/* IO Burst maintenence */
		for (i = 0; i < n; i++) {
			if (processes[i].ioTimeRemaining == -1) { //if process not in io do nothing
				continue;
			} else if (processes[i].ioTimeRemaining == 1) { //if process completes io then insert to readyQueue
				processes[i].ioTimeRemaining = -1;
				insert(&readyQueue, &(processes[i]));
				qStr = realloc(qStr, 4 + 2*readyQueue.itemCount*sizeof(char)*sizeof(char));
				printf("time %dms: Process %c completed I/O; added to ready queue %s\n", t, processes[i].processID,getQueue(readyQueue, qStr));
				fflush(stdout);
			} else { // if process is in io but not complete, decrement the counter
				processes[i].ioTimeRemaining -= 1;
			}
		}

		// start bringing a process into the cpu when nothing is happening - this happens at beginning of each ms
		//if nothing using cpu & process has been removed & nothing is being added, add a process to the cpu
		if (cpuTimeLeft == -1 && timeToRemove == 0 && entering == 0) { //process removed from cpu, nothing added yet
			if (!isEmpty(readyQueue)) { // as long as ready queue not empty
				
				entering = 1; //flag to show that a process is being brought into the cpu
				timeToBring = t_cs/2; // count down till you can bring it in
			}
		}

		// if a process was entering the cpu and is now in there, it can use the cpu
		if (entering == 1 && timeToBring == 0 && cpuTimeLeft == -1) {
			currentCPUProcess = pop(&readyQueue); // pop whatever is in front
      if (currentCPUProcess->cpuBurstTime < currentCPUProcess->timeRemaining)
      {
			  cpuTimeLeft = currentCPUProcess->cpuBurstTime;
      }
      else
      {
        cpuTimeLeft = currentCPUProcess->timeRemaining;
      }   
			qStr = realloc(qStr, 4 + 2*readyQueue.itemCount*sizeof(char)*sizeof(char));


      if (currentCPUProcess->timeRemaining == currentCPUProcess->cpuBurstTime)
      {   
          printf("time %dms: Process %c started using the CPU %s\n", t, currentCPUProcess->processID, getQueue(readyQueue, qStr));      
      }
      else
      {
          printf("time %dms: Process %c started using the CPU with %dms remaining %s\n", t, currentCPUProcess->processID,currentCPUProcess->timeRemaining, getQueue(readyQueue, qStr)); 
      }
			fflush(stdout);
			entering = 0;
      slice_time = 0;
		}

		//if a process completes its cpu burst, it must be removed from the cpu
		if (cpuTimeLeft == 0) {
			currentCPUProcess->numBursts -= 1;
			numContextSwitches += 1;
			cpuTimeLeft = -1;
			timeToRemove = t_cs/2;
      		currentCPUProcess->totalWaitTime-=t_cs/2;
			if (currentCPUProcess->numBursts > 0) {
				qStr = realloc(qStr, 4 + 2*readyQueue.itemCount*sizeof(char)*sizeof(char));
				if(currentCPUProcess->numBursts>1)
        		{
         			printf("time %dms: Process %c completed a CPU burst; %d bursts to go %s\n",t, currentCPUProcess->processID, 
					currentCPUProcess->numBursts, getQueue(readyQueue, qStr));
					fflush(stdout);
        		}
        		else
        		{
          		printf("time %dms: Process %c completed a CPU burst; %d burst to go %s\n",t, currentCPUProcess->processID, 
					currentCPUProcess->numBursts, getQueue(readyQueue, qStr));
          			fflush(stdout);
   			    }
        		currentCPUProcess->timeRemaining = currentCPUProcess->cpuBurstTime;
				fflush(stdout);
				currentCPUProcess->ioTimeRemaining = currentCPUProcess->ioTime + timeToRemove;
				qStr = realloc(qStr, 4 + 2*readyQueue.itemCount*sizeof(char)*sizeof(char));
				printf("time %dms: Process %c switching out of CPU; will block on I/O until time %dms %s\n",t, 
					currentCPUProcess->processID, t + currentCPUProcess->ioTimeRemaining, getQueue(readyQueue, qStr));
				fflush(stdout);
			} else {
				qStr = realloc(qStr, 4 + 2*readyQueue.itemCount*sizeof(char)*sizeof(char));
				printf("time %dms: Process %c terminated %s\n", t, currentCPUProcess->processID, getQueue(readyQueue, qStr));
				fflush(stdout);
			}
		}

		/* decrement context switch and cpu timers */
		if (cpuTimeLeft != -1) { // if the cpu is currently being used
			cpuTimeLeft -= 1; // decrement timer
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
		}

		//increment timer
		t += 1;
    
    if (cpuTimeLeft > 0)
    {
      slice_time+=1;
      currentCPUProcess->timeRemaining-=1;
    }
		// check if done
		// if nothing in cpu and readyQueue is empty
		if (cpuTimeLeft == -1 && isEmpty(readyQueue) && entering == 0 && timeToRemove == 0) {
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
    processes[i].totalTurnaroundTime= processes[i].totalWaitTime + (processes[i].cpuBurstTime*processes[i].numBursts);
		avgWaitTime += (float) processes[i].totalWaitTime;
    avgTurnaroundTime+=processes[i].totalTurnaroundTime;
	}
	avgWaitTime /= (float)numBursts;
	avgTurnaroundTime = avgWaitTime + avgBurstTime+(numContextSwitches/(t_cs/2));
  

	/* Open File */
	FILE* fp = fopen(outputFileName, "a");
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
