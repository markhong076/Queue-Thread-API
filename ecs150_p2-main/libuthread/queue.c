#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

// node in the queue
typedef struct node {
	void *data;
	void *next;
} * node_t;

struct queue {
	int size;		// allocated size of queue
	node_t front;	// front of queue
	node_t end;		// end of queue
};


queue_t queue_create(void)
{
	queue_t queue = malloc(sizeof *queue);

	// ERROR: allocation
	if (queue == NULL)
		return NULL;

	// init values
	queue->front = NULL;
	queue->end = NULL;
	queue->size = 0;

	return queue;
}

int queue_destroy(queue_t queue)
{
	// free each node in queue
	node_t curr_node = queue->front;
	node_t buf;
	while (curr_node != NULL) {
		buf = curr_node->next;
		free(curr_node);
		curr_node = buf;
	}

	// free pointer to queue struct
	free(queue);

	return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
	// init new_node
	node_t new_node = malloc(sizeof *new_node);
	// ERROR: allocation
	if (new_node == NULL)
		return -1;

	// set new_node values
	new_node->data = data;
	new_node->next = NULL;

	// add node to queue
	if (queue->size == 0) {
		queue->front = new_node;
		queue->end = new_node;
	}
	else {
		queue->end->next = new_node;
		queue->end = new_node;
	}

	// inc size
	queue->size++;

	return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
	if (queue == NULL || data == NULL || queue->size == 0)
		return -1;

	node_t return_node = queue->front;

	// set return pointer with last value in queue
	*data = return_node->data;

	// delete front node in queue
	queue->front = return_node->next;
	free(return_node);
	queue->size--;
	
	return 0;
}

int queue_delete(queue_t queue, void *data)
{
	if (queue == NULL || data == NULL)
		return -1;

	// find node
	node_t prev_node = NULL;
	node_t target_node = queue->front;
	while (target_node != NULL) {
		// break loop if data is found
		if (target_node->data == data)
			break;
		
		// otherwise continue loop with each node incremented
		prev_node = target_node;
		target_node = target_node->next;
	}

	// ERROR: item not found
	if (target_node == NULL)
		return -1;

	// delete target_node
	prev_node->next = target_node->next;
	free(target_node);
	queue->size--;

	return 0;
}

int queue_iterate(queue_t queue, queue_func_t func)
{
	if (queue == NULL || func == NULL)
		return -1;

	// iterate through queue and run func on each node's data
	node_t curr_node = queue->front;
	while (curr_node != NULL) {
		func(queue, curr_node->data);
		curr_node = curr_node->next;
	}

	return 0;
}

int queue_length(queue_t queue)
{
	if (queue == NULL)
		return -1;

	return queue->size;
}

