//#ifndef _SCHED_H
//#define _SCHED_H

#include "fs.h"

#define NR_TASKS 64
#define HZ 100

#define FIRST_TASK task_array[0]
#define LAST_TASK task_array[NR_TASKS-1]

//#include <linux/head.h>
//#include <linux/fs.h>
//#include <linux/mm.h>
//#include <signal.h>

#if (NR_OPEN > 32)
#error "Currently the close-on-exec-flags are in one word, max 32 files/proc"
#endif

#define TASK_RUNNING			0
#define TASK_INTERRUPTIBLE		1
#define TASK_UNINTERRUPTIBLE	2
#define TASK_ZOMBIE				3
#define TASK_STOPPED			4

#ifndef NULL
#define NULL ((void *) 0)
#endif

extern void set_current();
extern void init_task_array();
extern void free_task_array();
extern struct task_struct* create_task(long pid, long priority);
extern int copy_page_tables(unsigned long from, unsigned long to, long size);
extern int free_page_tables(unsigned long from, unsigned long size);

extern void sched_init(void);
extern void schedule(void);
extern void trap_init(void);
extern void panic(const char* str);
//extern int tty_write(unsigned minor,char * buf,int count);

typedef int (*fn_ptr)();


struct tss_struct {
	long	eip;
	long	eflags;
	long	eax, ecx, edx, ebx;
	long	esp;
	long	ebp;
	long	esi;
	long	edi;
	long	es;		/* 16 high bits zero */
	long	cs;		/* 16 high bits zero */
	long	ss;		/* 16 high bits zero */
	long	ds;		/* 16 high bits zero */
	long	fs;		/* 16 high bits zero */
	long	gs;		/* 16 high bits zero */
};

struct task_struct {
	/* these are hardcoded - don't touch */
	long state;	/* -1 unrunnable, 0 runnable, >0 stopped */
	long counter;
	long priority;
	long signal;
	//struct sigaction sigaction[32];
	long blocked;	/* bitmap of masked signals */
/* various fields */
	int exit_code;
	long pid, father;
	long start_time;
	/* file system info */
	unsigned short umask;
	struct m_inode* pwd;
	struct m_inode* root;
	struct file* filp[NR_OPEN];
	/* tss for this task */
	struct tss_struct tss;
};



extern struct task_struct* task_array[NR_TASKS];

extern struct task_struct* current;
extern long volatile jiffies;
extern long startup_time;

#define CURRENT_TIME (startup_time+jiffies/HZ)

extern void add_timer(long jiffies, void (*fn)(void));
extern void sleep_on(struct task_struct** p);
extern void interruptible_sleep_on(struct task_struct** p);
extern void wake_up(struct task_struct** p);


//#endif
