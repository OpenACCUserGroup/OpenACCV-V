#include "acc_testsuite.h"

bool possible_result(real_t * remaining_combinations, int length, real_t current_value, real_t test_value){
    if (length == 0){
        if (fabs(current_value - test_value) > PRECISION){
            return true;
        }
        else {
            return false;
        }
    }
    real_t * passed = (real_t *)malloc((length - 1) * sizeof(real_t));
    for (int x = 0; x < length; ++x){
        for (int y = 0; y < x; ++y){
            passed[y] = remaining_combinations[y];
        }
        for (int y = x + 1; y < length; ++y){
            passed[y - 1] = remaining_combinations[y];
        }
        if (possible_result(passed, length - 1, remaining_combinations[x] - current_value, test_value)){
            free(passed);
            return true;
        }
    }
    free(passed);
    return false;
}

int test(){
    int err = 0;
    srand(time(NULL));
    real_t *a = (real_t *)malloc(n * sizeof(real_t));
    real_t *totals = (real_t *)malloc(((n/10) + 1) * sizeof(real_t));

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
    }
    for (int x = 0; x < (n/10) + 1; ++x){
        totals[x] = 0;
    }

    #pragma acc data copyin(a[0:n]) copy(totals[0:(n/10) + 1])
    {
        #pragma acc parallel
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                #pragma acc atomic update
                    totals[x%((int) (n/10) + 1)] = a[x] - totals[x%((int) (n/10) + 1)];
            }
        }
    }
    int indexer = 0;
    real_t * passed = (real_t *)malloc(10 * sizeof(real_t));
    for (int x = 0; x < (n/10) + 1; ++x){
        indexer = x;
        while (indexer < n){
            passed[indexer/((int) (n/10) + 1)] = a[indexer];
            indexer += (n/10) + 1;
        }
        if (!(possible_result(passed, 10, 0, totals[x]))){
            err += 1;
        }
    }




    free(a);
    free(totals);
    free(passed);
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
