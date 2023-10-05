#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"
#include "queue.h"


typedef struct uthread_tcb {
	/* TODO Phase 2 */
	uthread_ctx_t *ctx;
	void *stack;
	int status;

} uthread_tcb;

// global variable thread control block (initialized on uthread_run())
queue_t ready_queue;
queue_t finished_queue;
uthread_tcb *prev_thread;
uthread_tcb *curr_thread;
uthread_tcb *idle_thread;

struct uthread_tcb *uthread_current(void)
{
	return curr_thread;
}

void uthread_yield(void)
{
	// switch to idle context
	queue_enqueue(ready_queue, curr_thread);
	prev_thread = curr_thread;
	curr_thread = idle_thread;
	uthread_ctx_switch(prev_thread->ctx, idle_thread->ctx);
}

void uthread_exit(void)
{
	// switch to idle context
	queue_enqueue(finished_queue, curr_thread);
	prev_thread = curr_thread;
	curr_thread = idle_thread;
	uthread_ctx_switch(prev_thread->ctx, idle_thread->ctx);
}

int uthread_create(uthread_func_t func, void *arg)
{
	/* TODO Phase 2 */
	uthread_tcb *thread = malloc(sizeof(*thread));
	uthread_ctx_t *ctx = malloc(sizeof(*ctx));;
	void *stack = uthread_ctx_alloc_stack();

	// ERROR: memory allocation
	if (thread == NULL || ctx == NULL || stack == NULL)
		return -1;

	// create context
	uthread_ctx_init(ctx, stack, func, arg);
	// ERROR: memory allocation
	if (ctx == NULL)
		return -1;

	thread->ctx = ctx;
	thread->stack = stack;

	// put created context in ready queue
	queue_enqueue(ready_queue, thread);

	return 0;
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	if (func == NULL)
		return -1;

	// start preempt
	preempt_start(preempt);
	
	// init globals for library
	ready_queue = queue_create();
	finished_queue = queue_create();

	// init global thread variables
	idle_thread = malloc(sizeof(*idle_thread));
	idle_thread->ctx = malloc(sizeof(*idle_thread->ctx));

	// create a new thread
	if (uthread_create(func, arg) == -1)
		return -1;

	// idle loop
	int loopNum = 0;
	while (true) {
		uthread_tcb *next_thread = malloc(sizeof(*next_thread));

		// get next thread ready to run
		loopNum++;
		// printf("loop number: %d, ready_queue has %d items\n", loopNum, queue_length(ready_queue));
		int dequeue_status = queue_dequeue(ready_queue, (void**) &next_thread);

		// if nothing left to run, exit
		if (dequeue_status == -1) {
			// deallocate everything and break idle loop
			queue_destroy(finished_queue);
			queue_destroy(ready_queue);

			break;
		}

		// switch context
		prev_thread = idle_thread;
		curr_thread = next_thread;
		uthread_ctx_switch(idle_thread->ctx, next_thread->ctx);
	}

	// stop preempt
	preempt_stop();

	return 0;
}

void uthread_block(void)
{
	queue_delete(ready_queue, curr_thread);
	prev_thread = curr_thread;
	curr_thread = idle_thread;
	uthread_ctx_switch(prev_thread->ctx, idle_thread->ctx);
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	queue_enqueue(ready_queue, uthread);
}
