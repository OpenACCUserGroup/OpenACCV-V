#include "acc_testsuite.h"

int test(){
    int err = 0;
    srand(time(NULL));
    const int tile_arg = ARRAYSIZE;
    real_t * a = (real_t *)malloc(n * sizeof(real_t));
    real_t * b = (real_t *)malloc(n * sizeof(real_t));
    real_t * c = (real_t *)malloc(n * sizeof(real_t));
    real_t * d2 = (real_t *)malloc(n * n * sizeof(real_t));
    real_t * d3 = (real_t *)malloc(n * n * n * sizeof(real_t));
    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = rand() / (real_t)(RAND_MAX / 10);
    }
    real_t temp = 0.0;
    #pragma acc data copy(a[0:n], b[0:n], c[0:n], d2[0:n*n]) 
    {
        #pragma acc parallel loop tile(*, *) reduction(+:temp)
        for (int x = 0; x < n; ++x){
            for (int y = 0; y < n; ++y){
                temp = 0;
                for (int z = 0; z < n; ++z){
                    temp += a[x] + b[y] + c[z];
                }
                d2[x * n + y] = temp;
            }
        }
    }
    for (int x = 0; x < n; ++x){
        for (int y = 0; y < n; ++y){
            temp = 0.0;
            for (int z = 0; z < n; ++z){
                temp += a[x] + b[y] + c[z];
            }
            if (fabs(temp - d2[x * n + y]) > PRECISION * n){
                err += 1;
            }
        }
    }

    #pragma acc data copyin(a[0:n], b[0:n], c[0:n]) copyout(d3[0:n*n*n])
    {
        #pragma acc parallel loop tile(tile_arg/10, tile_arg, tile_arg*2)
        for (int x = 0; x < n; ++x){
            for (int y = 0; y < n; ++y){
                for (int z = 0; z < n; ++z){
                    d3[(n * n * x) + (n * y) + z] = a[x] + b[x] + c[x];
                }
            }
        }
    }

    for (int x = 0; x < n; ++x){
        for (int y = 0; y < n; ++y){
            for (int z = 0; z < n; ++z){
                if (fabs(d3[(n * n * x) + (n * y) + z] - a[x] - b[x] - c[x]) > PRECISION * 2){
                    err = 1;
                }
            }
        }
    }

    free(a);
    free(b);
    free(c);
    free(d2);
    free(d3);
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
