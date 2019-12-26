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
extern struct task_struct** current;
extern struct task_struct* task_array[];


//
// inside variable
//
static volatile int flag = 0;

static pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;

void* job3(void* arg);
void* job4(void* arg);

/**
	the content of kernel_run().
	global variable should be initilized here.
	
	thread1: run for task scheduler.
	thread2: run for shell.
**/
void test_for_task_shell_thread() {
	/*init kernel variable*/
	init_task_array();
	init_cpu();

	//printf("%d\n", current->pid);
	current = task_array[0];

	create_task(123, 0);
	create_task(234, 1);
	create_task(345, 2);

	pthread_t tid1, tid2;

	/*run job1*/
	errno = pthread_create(&tid1, NULL, job3, NULL);
	if (errno) {
		perror("pthread_create");
		exit(EXIT_FAILURE);
	}

	/*run job2*/
	errno = pthread_create(&tid2, NULL, job4, NULL);
	if (errno) {
		perror("pthread_create");
		exit(EXIT_FAILURE);
	}

	///*lock main*/
	//errno = pthread_mutex_lock(&mutex1);
	//if (errno) {
	//	perror("pthread_mutex_lock");
	//	exit(EXIT_FAILURE);
	//}
	///*wait signal to rerun*/
	//errno = pthread_cond_wait(&cond1, &mutex1);
	//if (errno) {
	//	perror("pthread_cond_wait");
	//	exit(EXIT_FAILURE);
	//}
	///*unlock main*/
	//errno = pthread_mutex_unlock(&mutex1);
	//if (errno) {
	//	perror("pthread_mutex_unlock");
	//	exit(EXIT_FAILURE);
	//}
	//printf("<thread_main> wait finish\n");

	/*wait job1 to exit*/
	errno = pthread_join(tid1, NULL);
	if (errno) {
		perror("pthread_join");
		exit(EXIT_FAILURE);
	}
	/*wait job2 to exit*/
	errno = pthread_join(tid2, NULL);
	if (errno) {
		perror("pthread_join");
		exit(EXIT_FAILURE);
	}

	/*destroy cond and mutex*/
	errno = pthread_cond_destroy(&cond1);
	if (errno) {
		perror("pthread_cond_destroy");
		exit(EXIT_FAILURE);
	}
	errno = pthread_mutex_destroy(&mutex1);
	if (errno) {
		perror("pthread_mutex_destroy");
		exit(EXIT_FAILURE);
	}

	return 0;
}

void* job3(void* arg) {
	//printf("%d\n", current->pid);

	while (1) {
		/*loop for scheduler of the tasks*/
		while (flag == 0) {
			printf("<thread1> running in job1's while loop\n");
			schedule();
		}
		printf("<thread1> running after job1's loop\n");

		/*set lock*/
		errno = pthread_mutex_lock(&mutex1);
		if (errno) {
			perror("pthread_mutex_lock");
			exit(EXIT_FAILURE);
		}

		/*wait signal cond1 to re-run*/
		errno = pthread_cond_wait(&cond1, &mutex1);
		if (errno) {
			perror("pthread_cond_wait");
			exit(EXIT_FAILURE);
		}
		printf("<thread1> get signal from job4, and re-run while loop\n");

		/*release lock*/
		errno = pthread_mutex_unlock(&mutex1);
		if (errno) {
			perror("pthread_mutex_unlock");
			exit(EXIT_FAILURE);
		}

		printf("<thread1> reset flag = 0\n");
		flag = 0;
	}
	return NULL;
}

void* job4(void* arg) {
	Sleep(5000);

	printf("<thread2> set flag = 1\n");
	flag = 1;
	Sleep(2100);

	/*run something*/
	printf("<thread2> run something like shell command\n");
	Sleep(2000);

	printf("<thread2> send signal to job3\n");

	/*set lock*/
	errno = pthread_mutex_lock(&mutex1);
	if (errno) {
		perror("pthread_mutex_lock");
		exit(EXIT_FAILURE);
	}

	/*send signal cond1 to job3*/
	errno = pthread_cond_signal(&cond1);
	if (errno) {
		perror("pthread_cond_signal");
		exit(EXIT_FAILURE);
	}

	/*release lock*/
	errno = pthread_mutex_unlock(&mutex1);
	if (errno) {
		perror("pthread_mutex_unlock");
		exit(EXIT_FAILURE);
	}

	return NULL;
}