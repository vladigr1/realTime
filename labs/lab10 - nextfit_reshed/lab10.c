#include <conf.h>
#include <kernel.h>
#include "original.h"
#include <mem.h>

void complete_memory_report();

int xmain()
{
	int i,res;
	char *mem1,*mem2;
	for(i=0;i<NUM_SONS;++i){
        if((res = xfork() )== 0){
			sleep(i+1);
			printf("son: %d hi \n",i);
			return 0;
		}else{
			printf("parent: after generating son %d\n",i);
		}
	}
	if(res != 0){
		complete_memory_report();
		waitID(6);
		printf("parent: finished waiting to 6\n");
		complete_memory_report();//this report shows that their two blocks
		mem1 = getmem(4);
		mem2 = getmem(4);
		complete_memory_report();//this report  that for both blocks mem added
		freemem(mem1,4);
		freemem(mem2,4);
		//waitFirst();
		//printf("parent: finished waiting to first\n");
		//waitAll();
		//printf("parent: finished waiting to all\n");
	}
} // xmain

void complete_memory_report()
{
	int     ps;
	int index = 0;
	struct  mblock  *p, *q, *leftover;


	for ( q=&memlist, p=q->mnext ;
		(char *)p != NULL ;
		q=p, p=p->mnext )
	 {
	      unsigned endpoint, nextpoint, size;
	      index++;
	      printf("\nBlock number %d, starting point = %u, size = %u\n",
		     index, p, p->mlen); 
	      if (p->mnext != NULL)
	      {
		nextpoint = (unsigned) p->mnext;
		endpoint = (unsigned) ( ((unsigned)p) + p->mlen);
		size = nextpoint - endpoint; 
		printf("\nGap: from %u to %u, size = %u\n", 
		       endpoint,  nextpoint, size   );
	      }
	      else
		 printf("\n Final address = %u\n", 
		       ((unsigned)p)+(p->mlen));
	 } /* for */

 } /* complete memory report */
