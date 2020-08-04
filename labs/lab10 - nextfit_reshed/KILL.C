/* kill.c - kill */
/* 8086 version */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <q.h>
#include <sleep.h>

#include "original.h"

/*------------------------------------------------------------------------
 *  kill  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
SYSCALL kill(pid)
	int	pid;		     /* process to kill		*/
{
	struct	pentry	*pptr;	     /* points to proc. table for pid */
	int	ps;	             /* saved processor status	*/
    int pstate;
	//new
	int parentId,i;

	disable(ps);
	if (isbadpid(pid) || (pptr = &proctab[pid])->pstate==PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	//new
	//------------------------------------------------------------------------
	//sons
	if( (parentId = paTable[pid]) != 0){
		//common in all sons
		sonNumaTable[parentId]--;
		//kprintf("parentId : %d sonNumaTable[parentId]-- : %d\n",parentId,sonNumaTable[parentId]);
		paTable[pid] = 0;
		if((waitType[parentId] == WAIT_FIRST) || (waitType[parentId] == pid)){
			//for WAIT_FIRST the first son only will enter
			//for id when the id is match will enter here
			//because it has similar procedure we include them both here
			for(i=0;i<NPROC;++i){		//suspend sons
				if(i != pid && paTable[i] == parentId){ //let pid die else suspend
					 suspend(i);
				}
			}
			//waitType[parentId] = 0;		//cant allow two waits after wait_first
			resume(parentId);			//parent is ready
		}
	}
	if(waitType[parentId] == WAIT_ALL && sonNumaTable[parentId] == 0 ){
		resume(parentId);
		waitType[parentId] = 0;
	}
	//parents
	if(pid != 0 && sonNumaTable[pid] != 0 ){//common in all parents that are going to die and have sons
		//kprintf("parentId %d dies with sonNumaTable[parentId]-- : %d\n",pid,sonNumaTable[pid]);
		for(i=0;i<NPROC;++i){
				if(paTable[i] == pid){
					//kprintf("son resume %d\n");
					resume(i);
				}
			}
			waitType[parentId] = 0; //make sure
	}
	/*------------------------------------------------------------------------
	*/
	if (--numproc == 0)
		xdone();

	freestk(pptr->pbase, pptr->plen);

	pstate = pptr->pstate;
	pptr->pstate = PRFREE;	
	if (pstate == PRCURR)
	  resched();
        else	
         if (pstate == PRWAIT)
         {
            semaph[pptr->psem].semcnt++;
	    dequeue(pid);
         } /* PRWAIT */
         else
         if (pstate == PRREADY)
	    dequeue(pid);
         else
         if (pstate == PRSLEEP)
         {
           if ( q[pid].qnext < NPROC ) /* qnext is not tail */
                q[q[pid].qnext].qkey += q[pid].qkey;  /* Account for the 
                                                              pid delay */
                dequeue(pid);
               /* The sleep queue may now be empty, or have a new first */
            	if ( slnempty = nonempty(clockq) ) 
                         sltop = &firstkey(clockq);

         } /*  PRSLEEP */
         else; /* default: PRSUSP, PRRECV  */
	restore(ps);
	return(OK);

} /* kill */
