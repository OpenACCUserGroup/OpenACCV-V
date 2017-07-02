#include "acc_testsuite.h"

int test(){
    int err = 0;
    srand(time(NULL));
    real_t * a = (real_t *)malloc(10 * n * sizeof(real_t));
    real_t * b = (real_t *)malloc(10 * n * sizeof(real_t));
    real_t * c = (real_t *)malloc(10 * n * sizeof(real_t));
    real_t * totals = (real_t *)malloc(10 * sizeof(real_t));
    real_t temp;

    for (int x = 0; x < n; ++x){
        a[x] = (999.4 + rand() / (real_t)(RAND_MAX)) / 1000;
        b[x] = (999.4 + rand() / (real_t)(RAND_MAX)) / 1000;
        c[x] = 0.0;
    }


    #pragma acc data copyin(a[0:10*n], b[0:10*n]) copyout(c[0:10*n]) copy(totals[0:10])
    {
        #pragma acc parallel loop gang private(temp)
        for (int x = 0; x < 10; ++x){
            temp = 1;
            #pragma acc loop worker reduction(*:temp)
            for (int y = 0; y < n; ++y){
                temp *= a[x * n + y] + b[x * n + y];
            }
            totals[x] = temp;
            #pragma acc loop worker
            for (int y = 0; y < n; ++y){
                c[x * n + y] = (a[x * n + y] + b[x * n + y]) / totals[x];
            }
        }
    }
    
    for (int x = 0; x < 10; ++x){
        temp = 1;
        for (int y = 0; y < n; ++y){
            temp *= a[x * n + y] + b[x * n + y];
        }
        if (fabs(temp - totals[x]) > (temp / 2 + totals[x] / 2) * PRECISION){
            err += 1;
        }
        for (int y = 0; y < n; ++y){
            if (fabs(c[x * n + y] - ((a[x * n + y] + b[x * n + y]) / totals[x])) > PRECISION){
                err += 1;
            }
        }
    }

    free(a);
    free(b);
    free(c);
    free(totals);

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
