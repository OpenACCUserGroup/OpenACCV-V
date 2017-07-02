#include "acc_testsuite.h"

int test(){
    int err = 0;
    srand(time(NULL));
    char * a = (char *)malloc(10 * n * sizeof(char));
    char * b = (char *)malloc(10 * sizeof(char));
    real_t false_margin = pow(exp(1), log(.5)/n);
    for (int x = 0; x < 10 * n; ++x){
        if(rand() / (real_t)(RAND_MAX) > false_margin){
            a[x] = 1;
        }
        else{
            a[x] = 0;
        }
    }
    char temp = 0;
    #pragma acc data copyin(a[0:10*n]) copy(b[0:10])
    {
        #pragma acc parallel loop private(temp)
        for (int x = 0; x < 10; ++x){
            temp = 0;
            #pragma acc loop vector reduction(||:temp)
            for (int y = 0; y < n; ++y){
                temp = temp || a[x * n + y];
            }
            b[x] = temp;
        }
    }
    char found;
    for (int x = 0; x < 10; ++x){
        found = 0;
        for (int y = 0; y < n; ++y){
            if (a[x * n + y] &! 0){
                found = 1;
            }
        }
        if (found != b[x]){
            err = 1;
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
