#include "acc_testsuite.h"

int test(){
    int err = 0;
    srand(time(NULL));
    real_t *a = (real_t *)malloc(n * sizeof(real_t));
    real_t *b = (real_t *)malloc(n * sizeof(real_t));
    int *c = (int *)malloc(n * sizeof(int));
    int *distribution = (int *)malloc(10 * sizeof(int));
    int *distribution_comparison = (int *)malloc(10 * sizeof(int));

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
    }
    for (int x = 0; x < 10; ++x){
        distribution[x] = 0;
        distribution_comparison[x] = 0;
    }

    #pragma acc data copyin(a[0:n], b[0:n]) copy(distribution[0:10]) copyout(c[0:n])
    {
        #pragma acc parallel
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                #pragma acc atomic update capture
                {
                    c[x] = distribution[(int) (a[x]*b[x]/10)];
                    distribution[(int) (a[x]*b[x]/10)]++;
                }
            }
        }
    }

    for (int x = 0; x < n; ++x){
        distribution_comparison[(int) (a[x]*b[x]/10)]++;
    }
    for (int x = 0; x < 10; ++x){
        if (distribution_comparison[x] != distribution[x]){
            err += 1;
            break;
        }
    }

    bool found = false;
    for (int x = 0; x < 10; ++x){
        for (int y = 0; y < distribution_comparison[x]; ++y){
            for (int z = 0; z < n; ++z){
                if (c[z] == y && (int) (a[z]*b[z]/10) == x){
                    found = true;
                    break;
                }
            }
            if (!found){
                err++;
            }
            found = false;
        }
    }

    free(a);
    free(b);
    free(c);
    free(distribution);
    free(distribution_comparison);
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
