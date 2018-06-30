#include "acc_testsuite.h"

int test(){
    int err = 0;
    srand(time(NULL));
    unsigned int *a = (unsigned int *)malloc(n * sizeof(int));
    unsigned int *b = (unsigned int *)malloc(n * sizeof(int));

    for (int x = 0; x < n; ++x){
        a[x] = 1<<7;
        for (int y = 0; y < 7; ++y){
            if ((rand()/(unsigned int) (RAND_MAX)) > .5){
                b[x] += 1<<y;
            }
        }
    }

    #pragma acc data copyin(b[0:n]) copy(a[0:n])
    #pragma acc parallel
    {
        #pragma acc loop
        for (int x = 0; x < n; ++x){
            #pragma acc loop
            for (int y = 0; y < 7; ++y){
                if ((b[x]>>y)%2 == 1){
                    #pragma acc atomic
                        a[x] >>= 1;
                }
            }
        }
    }

    for (int x = 0; x < n; ++x){
        for (int y = 0; y < 7; ++y){
            if ((b[x]>>y)%2 == 1){
                a[x] <<= 1;
            }
        }
        if (a[x] != 1<<7){
            err += 1;
        }
    }

    free(a);
    free(b);
    return err;
}


int main()
{
  int i;			/* Loop index */
  int result;		/* return value of the program */
  int failed=0; 		/* Number of failed tests */
  int success=0;		/* number of succeeded tests */
  static FILE * logFile;	/* pointer onto the logfile */
  static const char * logFileName = "test_acc_lib_acc_wait.log";	/* name of the logfile */


  /* Open a new Logfile or overwrite the existing one. */
  logFile = fopen(logFileName,"w+");

  printf("######## OpenACC Validation Suite V %s #####\n", ACCTS_VERSION );
  printf("## Repetitions: %3d                       ####\n",REPETITIONS);
  printf("## Array Size : %.2f MB                 ####\n",ARRAYSIZE * ARRAYSIZE/1e6);
  printf("##############################################\n");
  printf("Testing test_acc_lib_acc_wait\n\n");

  fprintf(logFile,"######## OpenACC Validation Suite V %s #####\n", ACCTS_VERSION );
  fprintf(logFile,"## Repetitions: %3d                       ####\n",REPETITIONS);
  fprintf(logFile,"## Array Size : %.2f MB                 ####\n",ARRAYSIZE * ARRAYSIZE/1e6);
  fprintf(logFile,"##############################################\n");
  fprintf(logFile,"Testing test_acc_lib_acc_wait\n\n");

  for ( i = 0; i < REPETITIONS; i++ ) {
    fprintf (logFile, "\n\n%d. run of test_acc_lib_acc_wait out of %d\n\n",i+1,REPETITIONS);
    if (test() == 0) {
      fprintf(logFile,"Test successful.\n");
      success++;
    } else {
      fprintf(logFile,"Error: Test failed.\n");
      printf("Error: Test failed.\n");
      failed++;
    }
  }

  if(failed==0) {
    fprintf(logFile,"\nDirective worked without errors.\n");
    printf("Directive worked without errors.\n");
    result=0;
  } else {
    fprintf(logFile,"\nDirective failed the test %i times out of %i. %i were successful\n",failed,REPETITIONS,success);
    printf("Directive failed the test %i times out of %i.\n%i test(s) were successful\n",failed,REPETITIONS,success);
    result = (int) (((double) failed / (double) REPETITIONS ) * 100 );
  }
  printf ("Result: %i\n", result);
  return result;
}
