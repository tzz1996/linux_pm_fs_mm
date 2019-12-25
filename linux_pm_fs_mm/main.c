//#include <stdio.h>
//#include <stdlib.h>
//#include <time.h>
//
//#include "cpu.h"
//#include "sched.h"
//
//
//
//
//
//int main(void) {
//	test_for_task();
//
//	return 0;
//}



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
//#include <unistd.h>
//#include <sys/types.h>
//#include <sys/syscall.h>
#include <pthread.h>
#include <time.h>
#include <windows.h>

#define gettid() syscall(__NR_gettid)

static volatile int flag = 0;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void* job1(void* arg);
void* job2(void* arg);

int main(void) {
	//printf("++++++++++ entry thread_main (pid: %d, tid: %ld) ++++++++++\n", getpid(), gettid());

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
	//printf("++++++++++ entry thread_1 (pid: %d, tid: %ld) ++++++++++\n", getpid(), gettid());

	Sleep(500);

	errno = pthread_mutex_lock(&mutex);
	if (errno) {
		perror("pthread_mutex_lock");
		exit(EXIT_FAILURE);
	}

	printf("<thread_1> before: %d\n", flag);
	if (flag == 2) {
		errno = pthread_cond_signal(&cond);
		if (errno) {
			perror("pthread_cond_signal");
			exit(EXIT_FAILURE);
		}
	}
	flag = 1;
	printf("<thread_1> after: %d\n", flag);

	errno = pthread_mutex_unlock(&mutex);
	if (errno) {
		perror("pthread_mutex_unlock");
		exit(EXIT_FAILURE);
	}

	//printf("---------- leave thread_1 (pid: %d, tid: %ld) ----------\n", getpid(), gettid());
	return NULL;
}

void* job2(void* arg) {
	//printf("++++++++++ entry thread_2 (pid: %d, tid: %ld) ++++++++++\n", getpid(), gettid());

	Sleep(500);

	errno = pthread_mutex_lock(&mutex);
	if (errno) {
		perror("pthread_mutex_lock");
		exit(EXIT_FAILURE);
	}

	printf("<thread_2> before: %d\n", flag);
	if (flag == 1) {
		errno = pthread_cond_signal(&cond);
		if (errno) {
			perror("pthread_cond_signal");
			exit(EXIT_FAILURE);
		}
	}
	flag = 2;
	printf("<thread_2> after: %d\n", flag);

	errno = pthread_mutex_unlock(&mutex);
	if (errno) {
		perror("pthread_mutex_unlock");
		exit(EXIT_FAILURE);
	}

	//printf("---------- leave thread_2 (pid: %d, tid: %ld) ----------\n", getpid(), gettid());
	return NULL;
}
