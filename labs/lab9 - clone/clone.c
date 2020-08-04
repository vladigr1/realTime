//#include <create.c>
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include "original.h"

SYSCALL clone(char name[], int no_of_params,   int param1, ...){
	//		[bp+4], 		[bp+6],				[bp+8], [bp+10]  ....
	//ellipsis sucks
	int ps;
	int newPid;
	int *parmas;
	disable(ps);
	parmas = &param1;
	newPid  = create(paTable[currpid],proctab[currpid].plen,
	proctab[currpid].pprio,name,no_of_params,parmas[0],parmas[1],parmas[2],parmas[3] );	//pushed 
	if (newPid == SYSERR){
		restore(ps);
		return SYSERR;
	}
	resume(newPid);	//newPid will be the pid of new proc
	restore(ps);
	return OK;
}