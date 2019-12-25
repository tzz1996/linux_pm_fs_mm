#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
//#include <pthread.h>


#include "cpu.h"
#include "sched.h"



extern clock_t volatile time_now;
extern clock_t startup_time;



void test_for_hello() {
	printf("hello! linux\n");
}

void test_for_task() {
	init_task_array();

	schedule();
	printf("%p\n", task_array[0]);
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

void test_for_thread() {

}