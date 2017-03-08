#include <stdio.h>
#include <stdlib.h>
#include "algorithms.h"


void createQueue(myQueue* q) {
	q->queue = malloc(sizeof(processInfo));
	q->itemCount = 0;
	q->size = 1;
	printf("Created Queue!\n");
}

void insert(myQueue* q, processInfo process) {
	if (q->itemCount >= q->size) { /* Resize array by doubling size */
		q->size += 1;
		q->queue = realloc(q->queue, q->size*sizeof(processInfo));
	}

	q->queue[q->itemCount] = process;

	q->itemCount += 1;
}

void insertAt(myQueue* q, processInfo process, int position) {
	if (q->itemCount >= q->size) { /* Resize array by doubling size */
		q->size += 1;
		q->queue = realloc(q->queue, q->size*sizeof(processInfo));
	}

	int i = 0;
	int j = 0;
	for (i; i < q->size; i++) {
		if (i < position) {
			continue;
		} else if (i == position) {
			for (j = q->size-1; j > position; j--) {
				q->queue[j] = q->queue[j-1]; 
			}
			q->queue[position] = process;
			q->itemCount += 1;
			break;
		}
	}
}

char pop(myQueue* q) {
	printf("The size before popping is: %d\n", q->size);

	int i;
	char pid = q->queue[0].processID;
	for (i=1; i<q->itemCount; i++) {
		q->queue[i-1] = q->queue[i];
	}

	q->size -= 1;
	q->itemCount -= 1;

	q->queue = realloc(q->queue, q->size*sizeof(processInfo));

	return pid;
}

void freeQueue(myQueue* q) {
	free(q->queue);
}