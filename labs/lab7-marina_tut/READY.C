/* ready.c - ready */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>

/*------------------------------------------------------------------------
 *  ready  --  make a process eligible for CPU service
 *------------------------------------------------------------------------
 */
int	ready (pid)
	int	pid;			/* id of process to make ready	*/
{
	register struct	pentry	*pptr;

	pptr = &proctab[pid];
	pptr->pstate = PRREADY;
	//new
	//------------------------------------------------------------------------
	entery_tod[pid] = tod;
	 /*------------------------------------------------------------------------
	 */
	insert(pid,rdyhead,pptr->pprio);
	return(OK);
}
