/*Zachary Maurer, George Tsitsopoulos, Jose Guvenilir*/

#ifndef ALGORITHMS_H_
#define ALGORITHMS_H_

typedef struct {
	char processID;
	int arrivalTime;
	int cpuBurstTime;
	int numBursts;
	int ioTime;
	int ioTimeRemaining;
	int totalWaitTime;
	int totalTurnaroundTime;
	int timeRemaining;
} processInfo;

typedef struct {
	float avgBurstTime;
	float avgWaitTime;
	float avgTurnaroundTime;
	int numContextSwitches;
	int numPreemptions;
} algorithmStats;

typedef struct {
	processInfo** queue;
	int itemCount;
	int size;
} myQueue;

void fcfs(processInfo*, const int, const char*);
void rr(processInfo*, const int, const char*);
void srt(processInfo*, const int, const char*);

void createQueue(myQueue*);
void insert(myQueue*, processInfo*);
void insertAt(myQueue*, processInfo*);
processInfo* pop(myQueue*);
void freeQueue(myQueue*);
int isEmpty(myQueue);
char* getQueue(myQueue, char*);
int contains(myQueue, char);

#endif