#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include "original.h"

SYSCALL reserve_cpu(int pid){
	int ps;
	disable(ps);
	//kprintf("reserve_cpu: %d\n",pid);
	if(isbadpid(pid) || my_testin(myqhead,pid) == TRUE || pid == 0){
		restore(ps);
		return(SYSERR);
	}
	//kprintf("rsc pid : %d rcflag: %d tod: %ld rctod: %ld \n",pid,rcflag,tod,rctod);
	//suspend(pid);	//he is in new queue
	dequeue(pid);	//disconnect from current q
	enqueue(pid,myqtail);//he is in new queue  == not in rdyqueue
	
	if( rcflag != TRUE){	//means that the queues is set
		int lstInMyq = q[(myqtail)].qprev;
		rcflag = TRUE;
		//rctod = t od;
		if( tod +5 == rctime[lstInMyq]){
			rctime[pid] = rctime[lstInMyq] + 1;
		}else{
			rctime[pid] = tod +5;
		}
	}else{
		rctime[pid] = tod + 5;
	}
	//kprintf("rsc pid : rctime[pid]: %ld \n",pid,rctime[pid]);
	/* if(tod > (rctod + rcnum)){
			rcnum = 0;
	}
	
		//insert(pid, myqhead, (tod + reserve_MIN + rcnum));
		rctime[pid] = (tod + reserve_MIN + rcnum);
		rcnum++;
		rctod = tod; */

	restore(ps);
	return(OK);
}

int my_testin(head, key)
int head;				/* q index of head of list	*/
int key;				/* key to use for this process	*/
{
	int	next;			/* runs through list		*/
	int	prev;
	next = q[head].qnext;
	while (q[next].qkey < key){	/* tail has MAXINT as key	*/
		if(next == key){
			kprintf("test found\n");
			return TRUE;
		}
		next = q[next].qnext;
	}
	return(FALSE);
}
