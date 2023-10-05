#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

struct semaphore {
	int count;
	queue_t queue;
	queue_t priority_queue;
};

sem_t sem_create(size_t count)
{
	sem_t sem = malloc(sizeof(*sem));
	// ERROR: memory allocation
	if (sem == NULL)
		return NULL;

	sem->count = count;
	sem->queue = queue_create();
	sem->priority_queue = queue_create();

	return sem;
}

int sem_destroy(sem_t sem)
{
	if (sem == NULL || queue_length(sem->queue) > 0)
		return -1;

	free(sem->queue);
	free(sem);

	return 0;
}

int sem_down(sem_t sem)
{
	if (sem == NULL)
		return -1;

	struct uthread_tcb *curr_thread = uthread_current();

	// block if no resources available
	int block_count = 0;
	while (sem->count == 0) {

		// on first block, put in normal queue
		if (block_count == 0)
			queue_enqueue(sem->queue, curr_thread);
		// if blocked more than once, consider it starved, give higher priority
		else
			queue_enqueue(sem->priority_queue, curr_thread);

		// block thread
		uthread_block();
		block_count++;
	} 

	// dec count and return once a resource is taken
	sem->count--;

	return 0;
}

int sem_up(sem_t sem)
{
	if (sem == NULL)
		return -1;

	// inc count
	sem->count++;

	// if count was previously 0, give resource to next_thread
	if (sem->count == 1) {
		int dequeue_status;
		struct uthread_tcb *next_thread;

		// try to dequeue from priority_queue first
		dequeue_status = queue_dequeue(sem->priority_queue, (void**) &next_thread);

		// if priority_queue was empty, try normal queue
		if (dequeue_status == -1)
			dequeue_status = queue_dequeue(sem->queue, (void**) &next_thread);

		// if a thread was dequeued from either queue, unblock it
		if (dequeue_status != -1)
			uthread_unblock(next_thread);
		
	}

	return 0;
}

