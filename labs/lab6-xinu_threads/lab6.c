#include <conf.h>
#include <kernel.h>
#include <math.h>

unsigned long int fact1[32];
unsigned long int arr1[32],arr2[32],arr3[32];
char str[80];
unsigned long int num; // global received long int (n)
int mutex,mergemutex,mergecounter,mutexCounter;
int compute_factors(unsigned long int arr[],unsigned long int n);
void cf_singleThread(unsigned long int arr[],int size,int start,int *arrSizei);
int mergeArray(unsigned long int destination[],unsigned long int source[],int sourceSize,int i);

xmain()
{
  int top, top1, top2, i;
  unsigned long int n;
  char str1[80];	//didnt existed ended compl
  printf("Enter unsigned long integer:\n");
  scanf("%lu", &n);
  top =  compute_factors(fact1,n);
  if (top == 1)
{
    sprintf(str, "%lu is a prime.\n", n);
    printf(str);
  } // if
  else
  {
    sprintf(str,"%lu is not a prime, it factors:\n", n);
    printf(str);
    printf("%d factors:\n", top);
    strcpy(str,"");
    for(i=0; i < top; i++)
     {
      sprintf(str1, " %lu \n", fact1[i]);
      strcat(str, str1);
     } // for
    printf(str);
  } // else
} /* main */

int compute_factors(unsigned long int arr[],unsigned long int n){
	void cf_singleThread();
	int arrSize = 0,top1,top2,top3,j;
	if(n == 1 || n == 2){
		return 1;
	} //if
	
	num = n;
	while(num % 2 == 0){
		//printf("cur added 2\n");
		arr[arrSize++] = 2;
		num /= 2;
	}
	mutex = screate(1);
	mutexCounter = screate(1);
	mergecounter = 0;
	mergemutex = screate(0);
	
	//top2 = cf_singleThread(arr2,32,5);
	//top3 = cf_singleThread(arr3,32,7);
	resume(create(cf_singleThread,INITSTK,INITPRIO,"top1",4,arr1,32,3,&top1));
	resume(create(cf_singleThread,INITSTK,INITPRIO,"top2",4,arr2,32,5,&top2));
	resume(create(cf_singleThread,INITSTK,INITPRIO,"top3",4,arr3,32,7,&top3));
	wait(mergemutex);
	arrSize= mergeArray(arr,arr1,top1,arrSize);
	if(num != 1){
		arr[arrSize++] = num;
		num=1;
	}
	//printf("arrSize: %d\n",arrSize);
	arrSize= mergeArray(arr,arr2,top2,arrSize);
	//printf("arrSize: %d\n",arrSize);
	arrSize=mergeArray(arr,arr3,top3,arrSize);
	//printf("arrSize: %d\n",arrSize);
	 //test
	/* printf("n %d\n",num);
	printf("top1 %d\n",top1);
	printf("top2 %d\n",top2);
	printf("top3 %d\n",top3);
	printf("arrSize %d\n",arrSize);
	for(j=0;j<arrSize;++j){
		printf("%d ",arr[j]);
	} */
	
	return arrSize;
}
int mergeArray(unsigned long int destination[],unsigned long int source[],int sourceSize,int i){
	//i is the current placement of destination
	int j;
	for(j=0;j<sourceSize;j++){
		if(i < 32){
			//printf("j: %d ",j);
			destination[i] = source[j];
			++i;
		}else{
			//printf("Too many factors\n");
		}
	}
	return i;
}

void cf_singleThread(unsigned long int arr[],int size,int start,int *arrSizei){
	int i=0;
	unsigned long int cur= start,end;
	end = (long int) sqrtl((long double)num);
	while(cur<=end){
		if(num%cur == 0){
			if(i<size){
				wait(mutex);
				num/= cur;
				signal(mutex);
				//printf("cur added %d num = %d\n",cur,num);
				arr[i] = cur;
				i++;
				
			}else{
				printf("Too many factors their maybe more factors\n");
				*arrSizei =  i;
				return;
				
			}
		}else{
				cur = cur + 6;
		}//if(num%cur == 0)
		wait(mutex);
		end = (long int) sqrtl((long double)num);
		//printf("end: %ld num: %ld\n",end,num);
		signal(mutex);
	}
	wait(mutexCounter);
	//printf("finished start: %d\n",start);
	*arrSizei  = i;
	mergecounter++;
	//printf("mergecounter: %d\n",mergecounter );
	if(mergecounter >= 3){
		signal(mergemutex);
	}
	signal(mutexCounter);
}
