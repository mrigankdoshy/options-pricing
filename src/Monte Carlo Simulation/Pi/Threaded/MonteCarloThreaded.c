#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h> 
#define NUM_THREADS 100000         //number of threads
#define TOT_COUNT 10000055      //total number of iterations
#define BILLION  1E9; 

/**
A random number generator. 
Guidance from from http://stackoverflow.com/a/3067387/1281089
**/
float randomNumberGenerator(){

   int randomValue = rand(); //Generate a random number   
   float randomUnit = randomValue / (float) RAND_MAX; //make it between 0 and 1 
   return randomUnit;
}

/**
The task allocated to a thread
**/
void *doit(void *threadid)
{
   long longTid;
   longTid = (long)threadid;
   
   int tid = (int)longTid;       //obtain the integer value of thread id

   //using malloc for the return variable in order make
   //sure that it is not destroyed once the thread call is finished
   float *inCount = (float *)malloc(sizeof(float));
   *inCount=0;
   
   //get the total number of iterations for a thread
   float totalIterations= TOT_COUNT/NUM_THREADS;
   
   int counter = 0;
   
   //calculation
   for(counter = 0; counter < totalIterations; counter++) {
      float x = randomNumberGenerator();
      float y = randomNumberGenerator();
      
      float result = sqrt((x * x) + (y * y));
      
      if(result < 1) {
         *inCount+=1;         //check if the generated value is inside a unit circle
      }
      
   }
   
   //get the remaining iterations calculated by thread 0
   if(tid == 0){
      float remainder = TOT_COUNT % NUM_THREADS;
      
      for(counter = 0; counter < remainder; counter++) {
      float x = randomNumberGenerator();
      float y = randomNumberGenerator();
      
      float result = sqrt((x * x) + (y * y));
      
      if(result<1){
         *inCount+=1;         //check if the generated value is inside a unit circle
      }
     }
   }
   //printf("In count from #%d : %f\n",tid,*inCount);
   pthread_exit((void *)inCount);     //return the in count
}

int main(int argc, char *argv[])
{
   pthread_t threads[NUM_THREADS];
   int rc;
   long t;
   void *status;
   float total=0;

   // Calculate time taken by a request
  struct timespec requestStart, requestEnd;
  clock_gettime(CLOCK_REALTIME, &requestStart);
   
   for(t=0;t<NUM_THREADS;t++){
     rc = pthread_create(&threads[t], NULL, doit, (void *)t);
     if (rc){
       printf("ERROR; return code from pthread_create() is %d\n", rc);
       exit(-1);
     }
   }

   //join the threads
   for(t=0;t<NUM_THREADS;t++) {
           
      pthread_join(threads[t], &status);
      //printf("Return from thread %ld is : %f\n",t, *(float*)status);
      
      total+=*(float*)status;            //keep track of the total in count
   }
     clock_gettime(CLOCK_REALTIME, &requestEnd);
     // Calculate time it took
     double timespent = ( requestEnd.tv_sec - requestStart.tv_sec )
       + ( requestEnd.tv_nsec - requestStart.tv_nsec ) / BILLION;

     printf("Time taken: %lf\n", timespent);
     printf("Value for Pi: %f \n", 4*(total/TOT_COUNT));
   
   /* Last thing that main() should do */
   pthread_exit(NULL);
}