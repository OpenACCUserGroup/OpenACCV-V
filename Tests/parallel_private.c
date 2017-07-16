#include "acc_testsuite.h"

int test(){
    int err = 0;
    srand(time(NULL));
    real_t * a = (real_t *)malloc(10 * n * sizeof(real_t));
    real_t * b = (real_t *)malloc(10 * n * sizeof(real_t));
    real_t * c = (real_t *)malloc(n * sizeof(real_t));
    real_t * d = (real_t *)malloc(10 * sizeof(real_t));

    for (int x = 0; x < 10; ++x){
        for (int y = 0; y < n; ++y){
            a[x * n + y] = rand() / (real_t)(RAND_MAX / 10);
            b[x * n + y] = rand() / (real_t)(RAND_MAX / 10);
            c[y] = 0.0;
        }
        d[x] = 0.0;
    }

    #pragma acc enter data copyin(a[0:10*n], b[0:10*n], d[0:10])
    #pragma acc parallel num_gangs(10) private(c[0:n])
    {
        #pragma acc loop gang
        for (int x = 0; x < 10; ++x){
            #pragma acc loop worker
            for (int y = 0; y < n; ++y){
                c[y] = a[x * n + y] + b[x * n + y];
            }
            #pragma acc loop seq
            for (int y = 0; y < n; ++y){
                d[x] += c[y];
            }
        }
    }
    #pragma acc exit data copyout(d[0:10]) delete(a[0:10*n], b[0:10*n])

    real_t temp;
    for (int x = 0; x < 10; ++x){
        temp = 0.0;
        for (int y = 0; y < n; ++y){
            temp += a[x * n + y] + b[x * n + y];
        }
        if (fabs(temp - d[x]) > (2 * PRECISION * n)){
            err = 1;
        }
    }

    free(a);
    free(b);
    free(c);
    free(d);
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
  printf("Testing parallel_private\n\n");

  fprintf(logFile,"######## OpenACC Validation Suite V %s #####\n", ACCTS_VERSION );
  fprintf(logFile,"## Repetitions: %3d                       ####\n",REPETITIONS);
  fprintf(logFile,"## Array Size : %.2f MB                 ####\n",ARRAYSIZE * ARRAYSIZE/1e6);
  fprintf(logFile,"##############################################\n");
  fprintf(logFile,"Testing parallel_private\n\n");

  for ( i = 0; i < REPETITIONS; i++ ) {
    fprintf (logFile, "\n\n%d. run of parallel_private out of %d\n\n",i+1,REPETITIONS);
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

