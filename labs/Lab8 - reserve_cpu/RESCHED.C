/* resched.c - resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
//new headers
#include "original.h"
#include <sleep.h>

/*------------------------------------------------------------------------
 *  resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRCURR.
 *------------------------------------------------------------------------
 */
int	resched()
{
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */
	
//new we added
//------------------------------------------------------------
	int rcproc;
	rcproc = q[myqhead].qnext;
//------------------------------------------------------------
	optr = &proctab[currpid];
	if ( optr->pstate == PRCURR ) 
         {
		/* no switch needed if current prio. higher than next	*/
		/* or if rescheduling is disabled ( pcxflag == 0 )	*/
		if ( sys_pcxget() == 0 || lastkey(rdytail) < optr->pprio
                 || ( (lastkey(rdytail) == optr->pprio) && (preempt > 0) ) )
			return;
		/* force context switch */
		optr->pstate = PRREADY;
		insert(currpid,rdyhead,optr->pprio);
	} /* if */ 
        else if ( sys_pcxget() == 0 ) 
            {
		kprintf("pid=%d state=%d name=%s",
			currpid,optr->pstate,optr->pname);
		panic("Reschedule impossible in this state");
	    } /* else if */

	/* remove highest priority process at end of ready list */
	
//new we added
//------------------------------------------------------------
	if(rcflag == TRUE && tod >= rctime[rcproc]){ // or "==" instead of ">="
		kprintf("rshed rcproc : rctime[pid]: %ld \n",rcproc,rctime[rcproc]);
		//kprintf("get cput\n");
		nptr = &proctab[(currpid = getfirst(myqhead))]; //or "dequeue(rcproc)" instead of "qetlast"
		//kprintf("delete\n");
		//dequeue(currpid); //doesnt need because pre set with getlast
		//ready (currpid);	//return him to ready queue
		if(q[myqhead].qnext >= NPROC && rcflag == TRUE){
			rcflag = FALSE;
		}/* else{
			rctod = tod; // tod - 5 
		} */
	}else{
		nptr = &proctab[(currpid = getlast(rdytail))];
	}
/*old we added
//------------------------------------------------------------
nptr = &proctab[ (currpid = getlast(rdytail)) ];
--------------------------------------------------------------
*/	
	//kprintf("ended if currpid : %d rcflag: %d tod: %ld rctod: %ld \n",currpid,rcflag,tod,rctod);
	nptr->pstate = PRCURR;		/* mark it currently running	*/
	preempt = QUANTUM;		/* reset preemption counter	*/
	ctxsw(&optr->pregs,&nptr->pregs);
	//kprintf("ctxsw\n");
	//kprintf("ended if rcflg currpid : %d\n",currpid);

	/* The OLD process returns here when resumed. */
	return;
}
