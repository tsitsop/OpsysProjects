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
	//printf("Gonna insert something.\n");
	if (q->itemCount >= q->size) { /* Resize array by doubling size */
		//printf("Gotta resize.\n");
		q->size *= 2;
		//printf("size: %d\n", q->size);
		q->queue = realloc(q->queue, q->size*sizeof(processInfo));
		//printf("Successfully resized\n");
	}

	q->queue[q->itemCount] = process;

	//printf("inserted!\n");
	q->itemCount += 1;
	//printf("lkdsjfl;k\n");
}

void pop(myQueue q) {
	int i;
	for (i=1; i<q.itemCount; i++) {
		q.queue[i-1] = q.queue[i];
	}
}

void freeQueue(myQueue* q) {
	free(q->queue);
}