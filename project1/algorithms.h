#ifndef ALGORITHMS_H_
#define ALGORITHMS_H_

typedef struct {
	char processID;
	int arrivalTime;
	int cpuBurstTime;
	int numBursts;
	int ioTime;
} processInfo;

typedef struct {
	float avgBurstTime;
	float avgWaitTime;
	float avgTurnaroundTime;
	int numContextSwitches;
	int numPreemptions;
} algorithmStats;

typedef struct {
	processInfo* queue;
	int itemCount;
	int size;
} myQueue;

void fcfs(processInfo*, const int, const char*);
void rr(processInfo*, const int, const char*);
void srt(processInfo*, const int, const char*);

void createQueue(myQueue*);
void insert(myQueue*, processInfo);
char pop(myQueue*);
void freeQueue(myQueue*);

#endif