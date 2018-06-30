#include "acc_testsuite.h"

bool is_possible(real_t* a, real_t* b, real_t* c, int length, real_t prev){
    if (length == 0){
        return true;
    }
    real_t *passed_a = (real_t *)malloc((length - 1) * sizeof(real_t));
    real_t *passed_b = (real_t *)malloc((length - 1) * sizeof(real_t));
    real_t *passed_c = (real_t *)malloc((length - 1) * sizeof(real_t));
    for (int x = 0; x < length; ++x){
        if (fabs(c[x] - prev) < PRECISION){
            for (int y = 0; y < x; ++y){
                passed_a[y] = a[y];
                passed_b[y] = b[y];
                passed_c[y] = c[y];
            }
            for (int y = x + 1; y < length; ++y){
                passed_a[y - 1] = a[y];
                passed_b[y - 1] = b[y];
                passed_c[y - 1] = c[y];
            }
            if (is_possible(passed_a, passed_b, passed_c, length - 1, prev / (a[x] + b[x]))){
                free(passed_a);
                free(passed_b);
                free(passed_c);
                return true;
            }
        }
    }
    free(passed_a);
    free(passed_b);
    free(passed_c);
    return false;
}

int test(){
    int err = 0;
    srand(time(NULL));
    real_t *a = (real_t *)malloc(n * sizeof(real_t));
    real_t *b = (real_t *)malloc(n * sizeof(real_t));
    real_t *c = (real_t *)malloc(n * sizeof(real_t));
    real_t *totals = (real_t *)malloc((n/10 + 1) * sizeof(real_t));
    real_t *totals_comparison = (real_t *)malloc((n/10 + 1) * sizeof(real_t));

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0;
    }
    for (int x = 0; x < n/10 + 1; ++x){
        totals[x] = 1;
        totals_comparison[x] = 1;
    }

    #pragma acc data copyin(a[0:n], b[0:n]) copy(totals[0:n/10 + 1]) copyout(c[0:n])
    #pragma acc parallel
    {
        #pragma acc loop
        for (int x = 0; x < n; ++x){
            #pragma acc atomic update capture
            {
                c[x] = totals[x/10];
                totals[x/10] /= (a[x] + b[x]);
            }
        }
    }

    for (int x = 0; x < n; ++x){
        totals_comparison[x/10] /= a[x] + b[x];
    }
    for (int x = 0; x < 10; ++x){
        if (fabs(totals_comparison[x] - totals[x]) > PRECISION){
            err += 1;
            break;
        }
    }

    real_t *temp_a = (real_t *)malloc(10 * sizeof(real_t));
    real_t *temp_b = (real_t *)malloc(10 * sizeof(real_t));
    real_t *temp_c = (real_t *)malloc(10 * sizeof(real_t));
    int temp_iterator;
    int ab_iterator;
    for (int x = 0; x < n; x = x + 10){
        temp_iterator = 0;
        for (ab_iterator = x; ab_iterator < n && ab_iterator < x + 10;  ab_iterator+= 1){
            temp_a[temp_iterator] = a[ab_iterator];
            temp_b[temp_iterator] = b[ab_iterator];
            temp_c[temp_iterator] = c[ab_iterator];
            temp_iterator++;
        }
        if (!(is_possible(temp_a, temp_b, temp_c, temp_iterator, 1))){
            err++;
        }
    }

    free(a);
    free(b);
    free(c);
    free(temp_a);
    free(temp_b);
    free(temp_c);
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
