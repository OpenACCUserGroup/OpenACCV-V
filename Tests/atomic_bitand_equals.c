#include "acc_testsuite.h"

int test(){
    int err = 0;
    srand(time(NULL));
    int  *a = (int *)malloc(n * sizeof(int));
    int *totals = (int *)malloc((n/10 + 1) * sizeof(int));
    int *totals_comparison = (int *)malloc((n/10 + 1) * sizeof(int));

    for (int x = 0; x < n; ++x){
        for (int y = 0; y < 8; ++y){
            if (rand()/(real_t)(RAND_MAX) < .933){ //.933 gets close to a 50/50 distribution for a collescence of 10 values
                a[x] += 1<<y;
            }
        }
    }
    for (int x = 0; x < n/10 + 1; ++x){
        for (int y = 0; y < 8; ++y){
            totals[x] +=  1<<y;
        }

    }

    #pragma acc data copyin(a[0:n]) copy(totals[0:n/10 + 1])
    #pragma acc parallel
    {
        #pragma acc loop
        for (int x = 0; x < n; ++x){
            #pragma acc atomic
                totals[x%(n/10 + 1)] &= a[x];
        }
    }

    for (int x = 0; x < n; ++x){
        totals_comparison[x%(n/10 + 1)] &= a[x];
    }
    for (int x = 0; x < 10; ++x){
        if (fabs(totals_comparison[x] - totals[x]) > PRECISION){
            printf("%d != %d\n", totals_comparison[x], totals[x]);
            err += 1;
            break;
        }
    }

    free(a);
    free(totals);
    free(totals_comparison);
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
