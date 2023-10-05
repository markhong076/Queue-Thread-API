#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <queue.h>

#define TEST_ASSERT(assert)				\
do {									\
	printf("ASSERT: " #assert " ... ");	\
	if (assert) {						\
		printf("PASS\n");				\
	} else	{							\
		printf("FAIL\n");				\
		exit(1);						\
	}									\
} while(0)

/* Create */
void test_create(void)
{
	fprintf(stderr, "\n*** TEST create ***\n");

	TEST_ASSERT(queue_create() != NULL);
}

/* Enqueue/Dequeue simple */
void test_queue_simple(void)
{
	int data = 3, *ptr;
	queue_t q;

	fprintf(stderr, "\n*** TEST queue_simple ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data);
}

/* Enqueue/Dequeue simple_long */
void test_queue_simple_long(void)
{
	int *ptr;
	int insert_data[5] = {4, 9, 13, 23, 5};
	int expected_values[5] = {4, 9, 13, 23, 5};
	queue_t q;

	fprintf(stderr, "\n*** TEST queue_simple_long ***\n");

	q = queue_create();

	for (int i = 0; i < 5; i++) {
		queue_enqueue(q, &insert_data[i]);
	}

	for (int i = 0; i < 5; i++) {
		queue_dequeue(q, (void**) &ptr);
		TEST_ASSERT(*ptr == expected_values[i]);
	}
}

/* Enqueue/Dequeue complex */
void test_delete(void)
{
	int *ptr;
	int insert_data[5] = {1, 2, 3, 4, 5};
	int expected_values[4] = {1, 2, 4, 5};
	queue_t q;

	fprintf(stderr, "\n*** TEST queue_delete ***\n");

	q = queue_create();

	for (int i = 0; i < 5; i++) {
		queue_enqueue(q, &insert_data[i]);
	}

	// delete idx 2
	queue_delete(q, &insert_data[2]);

	for (int i = 0; i < 5; i++) {
		if (queue_dequeue(q, (void**) &ptr) == -1) {
			// this should only fail on the last iteration, since the queue only has 4 items now
			TEST_ASSERT(i == 4);
			break;
		};

		TEST_ASSERT(*ptr == expected_values[i]);
	}
}

/* delete value not found */
void test_delete_not_found(void)
{
	int *not_found_ptr = malloc(sizeof(*not_found_ptr));
	int *ptr;
	int insert_data[5] = {1, 2, 3, 4, 5};
	int expected_values[5] = {1, 2, 3, 4, 5};
	queue_t q;

	fprintf(stderr, "\n*** TEST queue_delete_not_found ***\n");

	q = queue_create();

	for (int i = 0; i < 5; i++) {
		queue_enqueue(q, &insert_data[i]);
	}

	// delete not_found_ptr
	int delete_result = queue_delete(q, &not_found_ptr);
	TEST_ASSERT(delete_result == -1);

	for (int i = 0; i < 5; i++) {
		queue_dequeue(q, (void**) &ptr);

		TEST_ASSERT(*ptr == expected_values[i]);
	}
}

/* iterate */
static void times2(queue_t queue, void *data) {
	queue = queue;
	int *a = (int*) data;

	*a *= 2;
}

void test_iterate(void)
{
	int *ptr;
	int insert_data[5] = {1, 2, 3, 4, 5};
	int expected_values[5] = {2, 4, 6, 8, 10};
	queue_func_t callback_func = times2;
	queue_t q;

	fprintf(stderr, "\n*** TEST queue_iterate ***\n");

	q = queue_create();

	for (int i = 0; i < 5; i++) {
		queue_enqueue(q, &insert_data[i]);
	}

	// iterate and multiply each value by 2
	queue_iterate(q, callback_func);

	for (int i = 0; i < 5; i++) {
		queue_dequeue(q, (void**) &ptr);
		TEST_ASSERT(*ptr == expected_values[i]);
	}
}

// queue_complex - a bunch of commands put together in one
void test_queue_complex(void)
{
	int *ptr;
	int insert_data[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	int dequeued_values[10];
	int expected_values[10] = {0, 4, 6, 4, 6};
	queue_func_t callback_func = times2;
	queue_t q;

	fprintf(stderr, "\n*** TEST queue_queue_complex ***\n");

	q = queue_create();

	// enqueue first 4 values
	for (int i = 0; i < 4; i++)
		queue_enqueue(q, &insert_data[i]);

	/* QUEUE: 0, 1, 2, 3 */
	/* DEQUEUE:  */

	// delete value = 1
	queue_delete(q, &insert_data[1]);

	/* QUEUE: 0, 2, 3 */
	/* DEQUEUE:  */

	// iterate and multiply each value by 2
	queue_iterate(q, callback_func);

	/* QUEUE: 0, 4, 6 */
	/* DEQUEUE:  */

	// dequeue 2 values
	for (int j = 0; j < 2; j++) {
		queue_dequeue(q, (void**) &ptr);
		dequeued_values[j] = *ptr;
	}

	/* QUEUE: 6 */
	/* DEQUEUE: 0, 4 */

	// enqueue 3 more values
	for (int i = 4; i < 7; i++)
		queue_enqueue(q, &insert_data[i]);

	/* QUEUE: 6, 4, 5, 6  */
	/* DEQUEUE: 0, 4 */

	// delete value = 5
	queue_delete(q, &insert_data[5]);

	/* QUEUE: 6, 4, 6 */
	/* DEQUEUE: 0, 4 */

	// dequeue 3 more values
	for (int j = 2; j < 5; j++) {
		queue_dequeue(q, (void**) &ptr);
		dequeued_values[j] = *ptr;
	}

	/* QUEUE:  */
	/* DEQUEUE: 0, 4, 6, 4, 6 */

	// final check
	for (int i = 0; i < 5; i++) {
		// printf("%d == %d\n", dequeued_values[i], expected_values[i]);
		TEST_ASSERT(dequeued_values[i] == expected_values[i]);
	}
}

// run all operations on empty queue
void test_empty_queue(void) {
	int *ptr;
	int *ptr_doesnt_exist = malloc(sizeof(*ptr_doesnt_exist));
	int insert_data[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	int dequeued_values[10];
	int expected_values[10] = {0};
	queue_func_t callback_func = times2;
	queue_t q = queue_create();

	fprintf(stderr, "\n*** TEST queue_empty_queue ***\n");

	// enqueue a 0, dequeue a 0, queue is now empty
	queue_enqueue(q, &insert_data[0]);
	queue_dequeue(q, (void**) &ptr);
	dequeued_values[0] = *ptr;

	// try to delete a value
	int delete_res = queue_delete(q, ptr_doesnt_exist);
	TEST_ASSERT(delete_res == -1);

	// try to run iterate
	int iterate_res = queue_iterate(q, callback_func);
	TEST_ASSERT(iterate_res == 0);

	int queue_len = queue_length(q);
	TEST_ASSERT(queue_len == 0);

	TEST_ASSERT(expected_values[0] == dequeued_values[0]);
}

int main(void)
{
	test_create();
	test_queue_simple();
	test_queue_simple_long();
	test_delete();
	test_delete_not_found();
	test_iterate();
	test_queue_complex();
	test_empty_queue();

	return 0;
}
