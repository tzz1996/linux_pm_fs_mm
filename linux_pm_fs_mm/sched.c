#include <stdio.h>
#include <Windows.h>
#include "cpu.h"
#include "sched.h"



//
// outside variable and function
//



//
// inside variable and function
//
struct task_struct* current = NULL;
struct task_struct* task_array[NR_TASKS];

struct cpu_register* cpu = NULL;

clock_t volatile time_now = 0;
clock_t startup_time = 0;


struct task_struct* create_task(long pid, long priority);
void init_task_array();
void free_task_array();
void schedule(void);
long sys_get_pid();
void another_switch_to(struct task_struct** next);
void switch_to(int next);
void cpu_init();
void task_run(struct task_struct** run);


/**
	create a new task which is in the task_array.

	@param  pid			the pid of the new task.
	@param  priority    the priority of the new task.

	@return the task_struct of new task.
**/
struct task_struct* create_task(long pid, long priority) {
	/*find a null task_array[]*/
	int null_idx = 0;
	for (; null_idx < NR_TASKS; null_idx++) {
		if (!task_array[null_idx]) break;
	}

	task_array[null_idx] = (struct task_struct*)malloc(sizeof(struct task_struct));
	task_array[null_idx]->state = 0;
	task_array[null_idx]->counter = priority + 10;
	task_array[null_idx]->priority = priority;
	task_array[null_idx]->signal = 0;
	task_array[null_idx]->blocked = 0;
	task_array[null_idx]->exit_code = 0;
	task_array[null_idx]->pid = pid;
	task_array[null_idx]->father = 0;
	task_array[null_idx]->start_time = 0;
	task_array[null_idx]->pwd = NULL;
	task_array[null_idx]->root = NULL;
	/*init for filp[]*/
	for (int i = 0; i < NR_OPEN; i++) {
		task_array[null_idx]->filp[i] = NULL;
		/*(struct file*)malloc(sizeof(struct file));
		new_task->filp[i]->f_mode = 0;
		new_task->filp[i]->f_flags = 0;
		new_task->filp[i]->f_count = 0;
		new_task->filp[i]->f_inode = NULL;
		new_task->filp[i]->f_pos = 0;*/
	}

	/*init for tss struct*/
	task_array[null_idx]->tss.eip = 0;
	task_array[null_idx]->tss.eflags = 0;
	task_array[null_idx]->tss.eax = 0;
	task_array[null_idx]->tss.ebx = 0;
	task_array[null_idx]->tss.ecx = 0;
	task_array[null_idx]->tss.edx = 0;
	task_array[null_idx]->tss.esp = 0;
	task_array[null_idx]->tss.ebp = 0;
	task_array[null_idx]->tss.esi = 0;
	task_array[null_idx]->tss.edi = 0;
	task_array[null_idx]->tss.es = 0;
	task_array[null_idx]->tss.cs = 0;
	task_array[null_idx]->tss.ss = 0;
	task_array[null_idx]->tss.ds = 0;
	task_array[null_idx]->tss.fs = 0;
	task_array[null_idx]->tss.gs = 0;

	return task_array[null_idx];
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
	int max_counter = 0;
	int max_priorty = 0;
	struct task_struct** p;
	struct task_struct** next;
	int next_idx = 0;

	while (1) {
		int i;
		for (i = 0; i < NR_TASKS; i++) {
			if (!task_array[i]) continue;
			if (task_array[i]->state == TASK_RUNNING && task_array[i]->counter > max_counter) {
				max_counter = task_array[i]->counter;
				next_idx = i;
			}
		}
		/*for (p = &LAST_TASK; p > & FIRST_TASK; --p) {
			if (!(*p)) continue;
			if ((*p)->state == TASK_RUNNING && (*p)->counter > max_counter) {
				max_counter = (*p)->counter;
			}
		}*/
		if (max_counter) {
			break;
		}
		/*for (p = &LAST_TASK; p > & FIRST_TASK; --p) {
			if (!(*p)) continue;
			(*p)->counter = (*p)->priority + 10;
		}*/
		for (i = 0; i < NR_TASKS; i++) {
			if (!task_array[i]) continue;
			task_array[i]->counter = task_array[i]->priority + 10;
		}
	}

	next = (struct task_struct**)&task_array[next_idx];
	//switch_to(next_idx);
	another_switch_to(next);
}


/**
	system call, get pid of current task.

	@return  the pid of current task.
**/
long sys_get_pid() {
	return current->pid;
}


void another_switch_to(struct task_struct** next) {
	/*change cpu register content*/
	cpu->eip = (*next)->tss.eip;
	cpu->eflags = (*next)->tss.eflags;
	cpu->eax = (*next)->tss.eax;
	cpu->ebx = (*next)->tss.ebx;
	cpu->ecx = (*next)->tss.ecx;
	cpu->edx = (*next)->tss.edx;
	cpu->esp = (*next)->tss.esp;
	cpu->ebp = (*next)->tss.ebp;
	cpu->esi = (*next)->tss.esi;
	cpu->edi = (*next)->tss.edi;
	cpu->es = (*next)->tss.es;
	cpu->cs = (*next)->tss.cs;
	cpu->ss = (*next)->tss.ss;
	cpu->ds = (*next)->tss.ds;
	cpu->fs = (*next)->tss.fs;
	cpu->gs = (*next)->tss.gs;

	/*run task witch next point to*/
	task_run(next);
}

void switch_to(int next) {
	/*cpu->eip = (*next)->tss.eip;
	cpu->eflags = (*next)->tss.eflags;*/
	cpu->eip = task_array[next]->tss.eip;

}

void task_run(struct task_struct** run) {
	(*run)->counter--;
	printf("running task pid: %d\n", (*run)->pid);
	Sleep(5000);
	
	return;
}

void init_cpu() {
	cpu = (struct cpu_register*)malloc(sizeof(struct cpu_register));
	cpu->eip = 0;
	cpu->eflags = 0;
	cpu->eax = 0;
	cpu->ebx = 0;
	cpu->ecx = 0;
	cpu->edx = 0;
	cpu->esp = 0;
	cpu->ebp = 0;
	cpu->esi = 0;
	cpu->edi = 0;
	cpu->es = 0;
	cpu->cs = 0;
	cpu->ss = 0;
	cpu->ds = 0;
	cpu->fs = 0;
	cpu->gs = 0;
}