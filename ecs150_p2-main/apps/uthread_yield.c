/*
 * Thread creation and yielding test
 *
 * Tests the creation of multiples threads and the fact that a parent thread
 * should get returned to before its child is executed. The way the printing,
 * thread creation and yielding is done, the program should output:
 *
 * thread1
 * thread2
 * thread3
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <uthread.h>

void thread3(void *arg)
{
	(void)arg;

	uthread_yield();
	printf("thread3\n");
}

void thread2(void *arg)
{
	(void)arg;

	uthread_create(thread3, NULL);
	uthread_yield();
	printf("thread2\n");
}

void thread1(void *arg)
{
	(void)arg;

	uthread_create(thread2, NULL);
	uthread_yield();
	printf("thread1\n");
	uthread_yield();
}

/*
 *	begin 1: running - 1, ready_queue - 
 *	1 creates 2: running - 1, ready_queue - 2
 *	yield 1: running - 2, ready_queue - 1
 *	2 creates 3 : running - 2, ready_queue - 1, 3
 *	yield 2 : running - 1, ready_queue - 3, 2
 *	print("thread1") : running - 1, ready_queue - 3, 2
 *	yield 1 : running - 3, ready_queue - 2, 1
 *  yield 3 : running - 2, ready_queue - 1, 3
 *	print("thread2") : running - 2, ready_queue - 1, 3
 *	exit 2 : running - 1, ready_queue - 3
 
 *	print("thread3") : running - 3, ready_queue - 1
 *	print("thread3") : running - 3, ready_queue - 1
 *  
*/

int main(void)
{
	uthread_run(false, thread1, NULL);
	return 0;
}
