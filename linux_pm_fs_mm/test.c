#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <pthread.h>
//#include "sched.c"


#include "cpu.h"
#include "sched.h"



//
// outside variable and function
//
extern clock_t volatile time_now;
extern clock_t startup_time;
extern struct task_struct* task_array[];

extern struct task_struct* create_task(long pid, long priority);

//
// inside variable and function
//
void test_for_hello() {
	printf("hello! linux\n");
}

void test_for_task() {
	init_task_array();
	init_cpu();

	create_task(123, 0);
	create_task(234, 1);
	create_task(345, 2);

	while (1) {
		schedule();
	}

	for (int i = 0; i < 10; i++) {
		printf("%p\n", task_array[i]);
	}
}

void test_for_cpu() {
	struct cpu_register* cpu = NULL;
	cpu = (struct cpu_register*)malloc(sizeof(struct cpu_register));
	cpu->cs = 1;

	free(cpu);
}

void test_for_time() {
	clock_t start, end;
	clock_t duration;
	start = clock();

	Sleep(2000);

	end = clock();
	duration = (end - start) / CLOCKS_PER_SEC;

	printf("start: %ld \n", start);
	printf("end: %ld \n", end);
	printf("duration: %ld \n", duration);
}

