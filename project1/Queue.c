#include <stdio.h>
#include <stdlib.h>
#include "algorithms.h"


void createQueue(myQueue* q) {
	q->queue = malloc(sizeof(processInfo*));
	q->itemCount = 0;
	q->size = 1;
	printf("Created Queue!\n");
}

void insert(myQueue* q, processInfo* process) {
	//printf("%s\n",getQueue(*q) );
	fflush(stdout);
	if (q->itemCount >= q->size) { /* Resize array by doubling size */
		q->size += 1;
		q->queue = realloc(q->queue, q->size*sizeof(processInfo*));

	}
	
	//printf("%s\n",getQueue(*q) );
	fflush(stdout);

	q->queue[q->itemCount] = process;

	q->itemCount += 1;

	//printf("%s\n\n",getQueue(*q) );
	fflush(stdout);
	
}

int contains(myQueue q, char c) {
	int i = 0;

	for (i = 0; i < q.itemCount; i++) {
		if (q.queue[i]->processID == c) {
			return 1;
		}
	}
	return 0;
}



// void insertAt(myQueue* q, processInfo process, int position) {
// 	if (q->itemCount >= q->size) { /* Resize array by doubling size */
// 		q->size += 1;
// 		q->queue = realloc(q->queue, q->size*sizeof(processInfo));
// 	}

// 	int i = 0;
// 	int j = 0;
// 	for (i; i < q->size; i++) {
// 		if (i < position) {
// 			continue;
// 		} else if (i == position) {
// 			for (j = q->size-1; j > position; j--) {
// 				q->queue[j] = q->queue[j-1]; 
// 			}
// 			q->queue[position] = process;
// 			q->itemCount += 1;
// 			break;
// 		}
// 	}
// }

processInfo* pop(myQueue* q) {
	//printf("The size before popping is: %d\n", q->size);

	int i;
	processInfo* process = q->queue[0];
	for (i=1; i<q->itemCount; i++) {
		q->queue[i-1] = q->queue[i];
	}

	q->size -= 1;
	q->itemCount -= 1;

	q->queue = realloc(q->queue, q->size*sizeof(processInfo*));

	return process;
}

void freeQueue(myQueue* q) {
	free(q->queue);
}

int isEmpty(myQueue q) {
	if (q.itemCount > 0) {
		return 0;
	}

	return 1;
}

char* getQueue(myQueue q, char* qStr) {
	if (q.itemCount == 0) {
		return "[Q <empty>]";
	}

	// printf("q.itemCount = %d\n", q.itemCount );

	int qStrSize = 4 + 2*q.itemCount*sizeof(char);
	// char* qStr = malloc(qStrSize*sizeof(char));

	//printf("sizeof(qstr)-2 = %d\n", (int)sizeof(qStr)-2);

	qStr[0] = '[';
	qStr[1] = 'Q';
	qStr[qStrSize-2] = ']';
	qStr[qStrSize-1] = '\0';

	// printf("entering loop\n");
	int i = 0, j = 0;

	for (i=2; i < qStrSize-2; i++) {
		if (i%2 == 0) {
			qStr[i] =  ' ';
		} else {
			qStr[i] = q.queue[j]->processID;
			j += 1;
		}
		// printf("%s\n", qStr);
	}


	return qStr;
}