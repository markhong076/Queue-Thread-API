/*
 * Simple hello world test
 *
 * Tests the creation of a single thread and its successful return.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <uthread.h>


int thread1_accessed1 = false;
int thread1_accessed2 = false;
int thread2_accessed1 = false;
int thread2_accessed2 = false;

void thread2(void *arg) 
{
	(void)arg;

	while (true) {
		printf("thread 2\n");
		thread2_accessed1 = true;

		if (thread2_accessed1)
			thread2_accessed2 = true;

		// break if thread 1 has been accessed twice
		if (thread1_accessed2) {
			printf("thread 1 has been accessed twice. Breaking thread 2 loop.\n");
			break;
		}
	}
}

void thread1(void *arg)
{
	(void)arg;

	uthread_create(thread2, NULL);

	// increment until both threads have been run for a little
	while (true) {
		printf("thread 1\n");
		thread1_accessed1 = true;

		if (thread1_accessed1)
			thread1_accessed2 = true;

		// break if thread 2 has been accessed twice
		if (thread2_accessed2) {
			printf("thread 2 has been accessed twice. Breaking thread 1 loop.\n");
			break;
		}
	}

}

int main(void)
{
	uthread_run(true, thread1, NULL);

	return 0;
}
