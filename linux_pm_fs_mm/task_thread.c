#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <pthread.h>
//#include "sched.c"


#include "cpu.h"
#include "sched.h"

//
// outside variable
//



//
// inside variable
//
static volatile int flag = 0;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void* job1(void* arg);
void* job2(void* arg);

void test_for_task_thread() {
	pthread_t tid1, tid2;
	errno = pthread_create(&tid1, NULL, job1, NULL);
	if (errno) {
		perror("pthread_create");
		exit(EXIT_FAILURE);
	}
	errno = pthread_create(&tid2, NULL, job2, NULL);
	if (errno) {
		perror("pthread_create");
		exit(EXIT_FAILURE);
	}

	printf("<thread_main> waiting for 1->2 or 2->1\n");
	errno = pthread_mutex_lock(&mutex);
	if (errno) {
		perror("pthread_mutex_lock");
		exit(EXIT_FAILURE);
	}
	errno = pthread_cond_wait(&cond, &mutex);
	if (errno) {
		perror("pthread_cond_wait");
		exit(EXIT_FAILURE);
	}
	errno = pthread_mutex_unlock(&mutex);
	if (errno) {
		perror("pthread_mutex_unlock");
		exit(EXIT_FAILURE);
	}
	printf("<thread_main> wait finish\n");

	errno = pthread_join(tid1, NULL);
	if (errno) {
		perror("pthread_join");
		exit(EXIT_FAILURE);
	}
	errno = pthread_join(tid2, NULL);
	if (errno) {
		perror("pthread_join");
		exit(EXIT_FAILURE);
	}

	errno = pthread_cond_destroy(&cond);
	if (errno) {
		perror("pthread_cond_destroy");
		exit(EXIT_FAILURE);
	}
	errno = pthread_mutex_destroy(&mutex);
	if (errno) {
		perror("pthread_mutex_destroy");
		exit(EXIT_FAILURE);
	}

	//printf("---------- leave thread_main (pid: %d, tid: %ld) ----------\n", getpid(), gettid());
	return 0;
}

void* job1(void* arg) {
	while (flag == 0) {
		printf("running in job1's while loop\n");
		Sleep(2000);
	}
	printf("running after job1's loop");

	/*run something*/

	return NULL;
}

void* job2(void* arg) {
	

	return NULL;
}