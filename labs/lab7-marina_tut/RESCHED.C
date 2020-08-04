/* resched.c - resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
//new headers
#include "original.h"

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
	//new var
	int next;
	optr = &proctab[currpid];
//new
	//------------------------------------------------------------------------
	for(next = q[rdyhead].qnext; next<NPROC; next = q[next].qnext){
		if(next != 0){
			q[next].qkey += 1;
			proctab[next].pprio +=1;
		}
	}
	//------------------------------------------------------------------------
	if ( optr->pstate == PRCURR ) 
         {
		/* no switch needed if current prio. higher than next	*/
		/* or if rescheduling is disabled ( pcxflag == 0 )	*/
		if ( sys_pcxget() == 0 || lastkey(rdytail) < optr->pprio
                 || ( (lastkey(rdytail) == optr->pprio) && (preempt > 0) ) )
			return;
		/* force context switch */
		optr->pstate = PRREADY;
//new
		//------------------------------------------------------------------------
		entery_tod[currpid] = tod;
		/*------------------------------------------------------------------------
		*/
		insert(currpid,rdyhead,optr->pprio);
	} /* if */ 
        else if ( sys_pcxget() == 0 ) 
            {
		kprintf("pid=%d state=%d name=%s",
			currpid,optr->pstate,optr->pname);
		panic("Reschedule impossible in this state");
	    } /* else if */

	/* remove highest priority process at end of ready list */

	nptr = &proctab[ (currpid = getlast(rdytail)) ];
	nptr->pstate = PRCURR;		/* mark it currently running	*/
	//preempt = QUANTUM;		/* reset preemption counter	*/
//new
	//------------------------------------------------------------------------
	
	preempt = 3*QUANTUM;		/* reset preemption counter	*/
	nptr->pprio = orig_prio[currpid];	//change the pprio back to the original
	
	//------------------------------------------------------------------------
	ctxsw(&optr->pregs,&nptr->pregs);

	/* The OLD process returns here when resumed. */
	return;
}
