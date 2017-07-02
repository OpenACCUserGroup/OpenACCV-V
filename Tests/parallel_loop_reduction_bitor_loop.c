#include "acc_testsuite.h"

int test(){
    int err = 0;
    srand(time(NULL));
    unsigned int * a = (unsigned int *)malloc(10 * n * sizeof(unsigned int));
    unsigned int * b = (unsigned int *)malloc(10 * n * sizeof(unsigned int));
    unsigned int * b_copy = (unsigned int *)malloc(10 * n * sizeof(unsigned int));
    unsigned int * c = (unsigned int *)malloc(10 * sizeof(unsigned int));

    real_t false_margin = pow(exp(1), log(.5)/n);
    unsigned int temp = 1;
    for (int x = 0; x < 10; ++x){
        for (int y = 0; y < n; ++y){
            b[x * n + y] = (unsigned int) rand() / (real_t)(RAND_MAX / 1000);
            b_copy[x * n + y] = b[x * n + y];
            for (int z = 0; z < 16; ++z){
                if (rand() / (real_t) RAND_MAX > false_margin){
                    temp = 1;
                    for (int i = 0; i < z; ++i){
                        temp = temp * 2;
                    }
                    a[x * n + y] += temp;
                }
            }
        }
    }
   
    #pragma acc data copyin(a[0:10*n]) copy(b[0:10*n], c[0:10])
    {
        #pragma acc parallel loop gang private(temp)
        for (int x = 0; x < 10; ++x){
            temp = 0;
            #pragma acc loop worker reduction(|:temp)
            for (int y = 0; y < n; ++y){
                temp = temp | a[x * n + y];
            }
            c[x] = temp;
            #pragma acc loop worker
            for (int y = 0; y < n; ++y){
                b[x * n + y] = b[x * n + y] + c[x];
            }
        }
    }
    unsigned int host_c;
    for (int x = 0; x < 10; ++x){
        host_c = a[x * n];
        for (int y = 1; y < n; ++y){
            host_c = host_c | a[x * n + y];
        }
        if (host_c != c[x]){
            err += 1;
        }
        for (int y = 0; y < n; ++y){
            if (b[x * n + y] != b_copy[x * n + y] + host_c){
                err += 1;
            }
        }
    }

    free(a);
    free(b);
    free(b_copy);
    free(c);
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
