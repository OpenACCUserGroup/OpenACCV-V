#include "acc_testsuite.h"

bool is_possible(int* a, int* b, int length, int prev){
    if (length == 0){
        return true;
    }
    int *passed_a = (int *)malloc((length - 1) * sizeof(int));
    int *passed_b = (int *)malloc((length - 1) * sizeof(int));
    for (int x = 0; x < length; ++x){
        if (b[x] == (a[x]|prev)){
            for (int y = 0; y < x; ++y){
                passed_a[y] = a[y];
                passed_b[y] = b[y];
            }
            for (int y = x + 1; y < length; ++y){
                passed_a[y - 1] = a[y];
                passed_b[y - 1] = b[y];
            }
            if (is_possible(passed_a, passed_b, length - 1, b[x])){
                free(passed_a);
                free(passed_b);
                return true;
            }
        }
    }
    free(passed_a);
    free(passed_b);
    return false;
}

int test(){
    int err = 0;
    srand(time(NULL));
    int *a = (int *)malloc(n * sizeof(int));
    int *b = (int *)malloc(n * sizeof(int));
    int *totals = (int *)malloc((n/10 + 1) * sizeof(int));
    int *totals_comparison = (int *)malloc((n/10 + 1) * sizeof(int));

    for (int x = 0; x < n; ++x){
        for (int y = 0; y < 8; ++y){
            if (rand()/(real_t)(RAND_MAX) > .933){ //.933 gets close to a 50/50 distribution for a collescence of 10 values
                a[x] += 1<<y;
            }
        }
    }
    for (int x = 0; x < n/10 + 1; ++x){
        totals[x] = 0;
        totals_comparison[x] = 0;
    }
    for (int x = 0; x < n; ++x){
        b[x] = 0;
    }

    #pragma acc data copyin(a[0:n]) copy(totals[0:n/10 + 1]) copyout(b[0:n])
    {
        #pragma acc parallel
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                #pragma acc atomic capture
                    b[x] = totals[x%(n/10 + 1)] |= a[x];
            }
        }
    }
    for (int x = 0; x < n; ++x){
        totals_comparison[x%(n/10 + 1)] |= a[x];
    }
    for (int x = 0; x < 10; ++x){
        if (fabs(totals_comparison[x] - totals[x]) > PRECISION){
            err += 1;
            break;
        }
    }
    int *temp_a = (int *)malloc(10 * sizeof(int));
    int *temp_b = (int *)malloc(10 * sizeof(int));
    int temp_indexer = 0;
    int absolute_indexer = 0;
    for (int x = 0; x < (n/10 + 1); ++x){
        for (absolute_indexer = x, temp_indexer = 0; absolute_indexer < n; absolute_indexer += n/10 + 1, temp_indexer++){
            temp_a[temp_indexer] = a[absolute_indexer];
            temp_b[temp_indexer] = b[absolute_indexer];
        }
        if (!(is_possible(temp_a, temp_b, temp_indexer, 0))){
            err += 1;
            printf("ERROR!\n");
        }
    }

    free(a);
    free(b);
    free(temp_a);
    free(temp_b);
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
