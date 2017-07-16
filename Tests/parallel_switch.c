#include "acc_testsuite.h"

int test(){
    int err = 0;
    srand(time(NULL));
    int * a = (int *)malloc(n * sizeof(int));
    real_t * b = (real_t *)malloc(n * sizeof(real_t));
    real_t * b_host = (real_t *)malloc(n * sizeof(real_t));
    real_t * c = (real_t *)malloc(n * sizeof(real_t));


    for (int x = 0; x < n; ++x){
        a[x] = floor(rand() / (real_t)(RAND_MAX / 10));
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        b_host[x] = b[x];
        c[x] = 0.0;
    }
    #pragma acc enter data copyin(a[0:n], b[0:n], c[0:n])
    #pragma acc parallel present(a[0:n], b[0:n], c[0:n])
    {
        #pragma acc loop
        for (int x = 0; x < n; ++x){
            switch(a[x]){
                case 0:
                    c[x] = b[x] * b[x];
                    break;
                case 1:
                    c[x] = b[x] / b[x];
                    break;
                case 2:
                    b[x] = b[x] / 2;
                default:
                    c[x] = a[x] + b[x];


            }
        }
    }
    #pragma acc exit data delete(a[0:n], b[0:n]) copyout(c[0:n])

    real_t tempc = 0.0;
    for (int x = 0; x < n; ++x){
        switch(a[x]){
            case 0:
                tempc = b_host[x] * b_host[x];
                break;
            case 1:
                tempc = b_host[x] / b_host[x];
                break;
            case 2:
                b_host[x] = b_host[x] / 2;
            default:
                tempc = a[x] + b_host[x];
        }
        if (fabs(c[x] - tempc) > PRECISION){
            err = 1;
        }
    }

    free(a);
    return err;
}


int main()
{
  int i;                        /* Loop index */
  int result;           /* return value of the program */
  int failed=0;                 /* Number of failed tests */
  int success=0;                /* number of succeeded tests */
  static FILE * logFile;        /* pointer onto the logfile */
  static const char * logFileName = "OpenACC_testsuite.log";        /* name of the logfile */


  /* Open a new Logfile or overwrite the existing one. */
  logFile = fopen(logFileName,"w+");

  printf("######## OpenACC Validation Suite V %s #####\n", ACCTS_VERSION );
  printf("## Repetitions: %3d                       ####\n",REPETITIONS);
  printf("## Array Size : %.2f MB                 ####\n",ARRAYSIZE * ARRAYSIZE/1e6);
  printf("##############################################\n");
  printf("Testing parallel_switch\n\n");

  fprintf(logFile,"######## OpenACC Validation Suite V %s #####\n", ACCTS_VERSION );
  fprintf(logFile,"## Repetitions: %3d                       ####\n",REPETITIONS);
  fprintf(logFile,"## Array Size : %.2f MB                 ####\n",ARRAYSIZE * ARRAYSIZE/1e6);
  fprintf(logFile,"##############################################\n");
  fprintf(logFile,"Testing parallel_switch\n\n");

  for ( i = 0; i < REPETITIONS; i++ ) {
    fprintf (logFile, "\n\n%d. run of parallel_switch out of %d\n\n",i+1,REPETITIONS);
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

