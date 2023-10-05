# ECS 150 Project 2 Report
## Erik Young and Mark Hong

November 3 2022

# Project Phases
* Phase 1: [Queue API](#queue-api)
* Phase 2: [Uthread API](#uthread-api)
* Phase 3: [Semaphore API](#semaphore-api)
* Phase 4: [Preemption](#preemption)

# Queue API
In order to create a FIFO queue, we decided to use a linked list. This decision was based on the fact that we didn't want to constantly dynamically allocate memory to use a data structure like an array. Using a linked list, we didn't really have to worry about memory leaks. Creating and destroying a queue was fairly easy to understand. 

Each value enqueued in the queue is held in a struct that also points to the next node in the queue. Using this *linked list* type design also allows us to delete the node at the front of the queue, without having to shift down all other elements (like what would have needed to be done with an array implementation). This *linked list* design also allowed us to easily delete a node in the middle of the queue, without having the same shifting of elements problem.

# Uthread API
Phase 2 was probably the hardest phase. In order to create the uthread API, we took advantage of using global variables since all threads share the same memory space. We defined control block pointers prev_thread, curr_thread, and idle_thread. In addition, we created queue_t variables for the ready and finished queue. It is important to note that we found it rather helpful to implement a finished queue although it is unnecessary. We could have easily achieved the same task by using a counter variable, but we would have had to deallocate the memory much more frequently than we needed to. 

We found it very useful to refer to the pthread library to implement our own functions. However, we did find it fairly difficult to implement uthread_run. In particular, we found it hard to understand the idle and initial thread. We realized that the idle thread refers to the main idle loop and we just needed to create a while loop that doesn't exit until there are no more threads in the ready queue. 

In addition, we also recognized that uthread_yield and uthread_exit can be implemented very similarly to one another. uthread_yield simply adds the current thread to the end of the ready queue and points back to the idle thread. On the other hand, uthread_exit simply adds the current thread to the finished queue and points back to the idle thread afterwards. These functions were easy to implement once we recognized the similarity between them.

# Semaphore API
Some elements of the semaphore API were fairly similar to the uthread API. sem_create and sem_destroy were these elements. Again, these functions were relatively easy to create, just because of how self explanatory they were. Using our linekd list, we simply just initialized the variables we defined in the semaphore structure. The semaphore structure consists of int count, queue, priority_queue. 

The trickiest part of the semaphore API by far was implementing the sem_down and sem_up functions. Before creating these functions, we first had to implement the uthread_block and uthread_unblock functions in uthread.c. Our thought process was this: when we block a ctx, we get rid of that thread from the ready queue. This would then throw us back into the idle loop to await our next instruction. This thread will remain blocked until it is put back into the ready queue. When this happens, we simply just call uthread_unblock - thus, the thread is back in the end of the ready queue. 

Once we had these two functions implemented, we wrote sem_down and sem_up. If sem_down and the allocated resources, which we called block_count, was 0, then we simply just blocked the thread.

We handled the particular edge case that was talked about in the project directions by adding an extra resource check before allowing a now unblocked thread from resuming. We did this by changing the statement to decide if a thread should be blocked from:
```c
if (sem->count == 0) {
    ...
}
```
to:
```c
while (sem->count == 0) {
    ...
}
```
This will again check the count before leaving the while loop upon a thread resuming its execution. If the previously available resource has been taken, it will re-enter the blocked queue. 

Since this process subjects a blocked thread to being starved, we added a *priority_queue*. This *priority_queue* holds threads that have been blocked more than once. The idea of this *priority_queue* is to be able to essentially put a thread that has been blocked more than once to the front of the line. When the semaphore has a resource available, it will first check the priority queue for any threads that want a resource, and if there are no threads in the priority queue, then it will check the normal queue. With this implementation, we should have no race conditions, and no starvation.

# Preemption
This 4th and final phase took the least amount of time of all of them. We set up an alarm as requested to trigger a handler function to run 100 times per second. This handler function would call `uthread_yield()`, effectively yielding the current thread. Since this happens 100 every second, and every yielded thread is queued to the back of the *ready queue*, we essentially cycle through thread, executing each thread only for 1/100th of a second before switching to the next thread in the queue again. 

To test this, we created a `test_preempt.c` tester file that creates 2 threads. These threads manipulate global variables to check whether the other thread has been run. If both threads have been run twice (to show a back and forth from the preemption), and set their corresponding flags to say so, they will finally quit. If the program quits, it passes.
