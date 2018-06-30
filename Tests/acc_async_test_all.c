#include "acc_testsuite.h"

int test(){
    int err = 0;
    real_t **a = (real_t **)malloc(10 * sizeof(real_t));
    real_t **b = (real_t **)malloc(10 * sizeof(real_t));
    real_t **c = (real_t **)malloc(10 * sizeof(real_t));
    real_t **d = (real_t **)malloc(10 * sizeof(real_t));
    real_t **e = (real_t **)malloc(10 * sizeof(real_t));
    for (int x = 0; x < 10; ++x){
        a[x] = (real_t *)malloc(n * sizeof(real_t));
        b[x] = (real_t *)malloc(n * sizeof(real_t));
        c[x] = (real_t *)malloc(n * sizeof(real_t));
        d[x] = (real_t *)malloc(n * sizeof(real_t));
        e[x] = (real_t *)malloc(n * sizeof(real_t));
        for (int y = 0; y < n; ++y){
            a[x][y] = rand() / (real_t)(RAND_MAX / 10);
            b[x][y] = rand() / (real_t)(RAND_MAX / 10);
            c[x][y] = 0;
            d[x][y] = rand() / (real_t)(RAND_MAX / 10);
            e[x][y] = 0;
        }
    }

    #pragma acc enter data create(a[0:10][0:n], b[0:10][0:n], c[0:10][0:n], d[0:10][0:n], e[0:10][0:n])
    for (int x = 0; x < 10; ++x){
        #pragma acc update device(a[x:1][0:n], b[x:1][0:n], d[x:1][0:n]) async(x)
        #pragma acc parallel present(a[x:1][0:n], b[x:1][0:n], c[x:1][0:n]) async(x)
        {
            #pragma acc loop
            for (int y = 0; y < n; ++y){
                c[x][y] = a[x][y] + b[x][y];
            }
        }
        #pragma acc parallel present(c[x:1][0:n], d[x:1][0:n], e[x:1][0:n]) async(x)
        {
            #pragma acc loop
            for (int y = 0; y < n; ++y){
                e[x][y] = c[x][y] + d[x][y];
            }
        }
        #pragma acc update host(e[x:1][0:n]) async(x)
    }

    while(!acc_async_test_all());
    for (int x = 0; x < 10; ++x){
        for (int y = 0; y < n; ++y){
            if (fabs(e[x][y] - (a[x][y] + b[x][y] + d[x][y])) > PRECISION){
                err += 1;
            }
        }
    }

    for (int x = 0; x < 10; ++x){
        free(a[x]);
        free(b[x]);
        free(c[x]);
        free(d[x]);
        free(e[x]);
    }
    free(a);
    free(b);
    free(c);
    free(d);
    free(e);

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
