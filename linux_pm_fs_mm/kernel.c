#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <pthread.h>

#include "cpu.h"
#include "sched.h"


clock_t volatile time_now;
clock_t startup_time;

extern void fs_init();
void time_init();

/**
	this is the function that run the linux kernel.

	@param  NULL

	@return void
**/
void kernel_run() {
	time_init();
	//fs_init();

	while (1) {
		schedule();
	}
}

void time_init() {
	startup_time = clock();
}