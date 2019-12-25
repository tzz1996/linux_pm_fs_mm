#ifndef _CPU_H
#define _CPU_H

struct cpu_register {
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

#endif