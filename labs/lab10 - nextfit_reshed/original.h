
#define NUM_SONS 3
#define WAIT_ALL -1
#define WAIT_FIRST -2

#ifndef	NPROC					/* set the number of processes	*/
#define	NPROC		30			/*  allowed if not already done	*/
#endif

extern int paTable[NPROC];		/* pid[i].parent == paTable[i]*/
extern int sonNumaTable[NPROC];	/* pid[i].numSons == sonNumaTable[i]*/
extern int waitType[NPROC];			/* wait type : 
												 0 no wait
												-1 waitAll 
												-2 waitFirst 
												pid waitID*/
extern struct	mblock *curBlock;