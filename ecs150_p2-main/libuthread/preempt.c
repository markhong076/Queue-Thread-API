#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include <unistd.h>

#include "private.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100

struct itimerval itimer;

void preempt_disable(void)
{
	sigset_t block_alarm;

	sigemptyset(&block_alarm);
	sigaddset(&block_alarm, SIGVTALRM);

	sigprocmask(SIG_BLOCK, &block_alarm, NULL);
}

void preempt_enable(void)
{
	/* TODO Phase 4 */
	sigset_t block_alarm;

	sigemptyset(&block_alarm);
	sigaddset(&block_alarm, SIGVTALRM);

	sigprocmask(SIG_UNBLOCK, &block_alarm, NULL);
}


// handler for timer alarm
void alarm_handler() {
	uthread_yield();
}

void preempt_start(bool preempt)
{
	struct sigaction act = { 0 };

	// do nothing if preempt == false
	if (!preempt)
		return;

	/* Install handler for segfaults */
	act.sa_handler = &alarm_handler;
    sigaction(SIGVTALRM, &act, NULL);

	// set timer
	itimer.it_value.tv_usec = 100 * HZ;
	itimer.it_interval.tv_usec = 100 * HZ;
	setitimer(ITIMER_VIRTUAL, &itimer, NULL);

}

void preempt_stop(void)
{
	itimer.it_value.tv_sec = 0;
	itimer.it_interval.tv_sec = 0;
	setitimer(ITIMER_VIRTUAL, &itimer, NULL);

}

