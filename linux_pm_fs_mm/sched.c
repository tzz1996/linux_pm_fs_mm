#include <stdio.h>
#include "sched.h"


struct task_struct* current = NULL;
struct task_struct* task_array[NR_TASKS];

clock_t volatile time_now = 0;
clock_t startup_time = 0;


struct task_struct* create_task(long pid, long priority);
void init_task_array();
void free_task_array();
void schedule(void);
long sys_get_pid();


/**
	create a new task which is in the task_array.

	@param  pid			the pid of the new task.
	@param  priority    the priority of the new task.

	@return the task_struct of new task.
**/
struct task_struct* create_task(long pid, long priority) {
	struct task_struct* new_task = (struct task_struct*)malloc(sizeof(struct task_struct));
	new_task->state = 0;
	new_task->counter = 0;
	new_task->priority = priority;
	new_task->signal = 0;
	new_task->blocked = 0;
	new_task->exit_code = 0;
	new_task->pid = pid;
	new_task->father = 0;
	new_task->start_time = 0;
	new_task->pwd = NULL;
	new_task->root = NULL;
	/*init for filp[]*/
	for (int i = 0; i < NR_OPEN; i++) {
		new_task->filp[i] = NULL;
		/*(struct file*)malloc(sizeof(struct file));
		new_task->filp[i]->f_mode = 0;
		new_task->filp[i]->f_flags = 0;
		new_task->filp[i]->f_count = 0;
		new_task->filp[i]->f_inode = NULL;
		new_task->filp[i]->f_pos = 0;*/
	}

	/*init for tss struct*/
	new_task->tss.eip = 0;
	new_task->tss.eflags = 0;
	new_task->tss.eax = 0;
	new_task->tss.ebx = 0;
	new_task->tss.ecx = 0;
	new_task->tss.edx = 0;
	new_task->tss.esp = 0;
	new_task->tss.ebp = 0;
	new_task->tss.esi = 0;
	new_task->tss.edi = 0;
	new_task->tss.es = 0;
	new_task->tss.cs = 0;
	new_task->tss.ss = 0;
	new_task->tss.ds = 0;
	new_task->tss.fs = 0;
	new_task->tss.gs = 0;

	return new_task;
}

/**
	init the global task array.
	make task_array[0] -- task_array[NR_TASKS - 1] NULL.
**/
void init_task_array() {
	for (int i = 0; i < NR_TASKS; i++) {
		task_array[i] = NULL;
	}
}

/**
	free the memory of filp of each task, and free task.
**/
void free_task_array() {
	for (int i = 0; i < NR_TASKS; i++) {
		for (int j = 0; j < NR_OPEN; j++) {
			if (task_array[i]->filp[j])
				free(task_array[i]->filp[j]);
		}
		if (task_array[i])
			free(task_array[i]);
	}
}

/**
	scheduler of task.
**/
void schedule(void) {
	struct task_struct** p;

	for (p = &LAST_TASK; p > &FIRST_TASK; --p) {
		if ((*p))
			printf("founed ");
	}
}


/**
	system call, get pid of current task.

	@return  the pid of current task.
**/
long sys_get_pid() {
	return current->pid;
}