#include "acc_testsuite.h"

int test(){
    int err = 0;
    srand(time(NULL));
    
    real_t * a = (real_t *)malloc(1024 * sizeof(real_t));
    real_t * b = (real_t *)malloc(1024 * sizeof(real_t));

    for (int x = 0; x < 1024; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = 0.0;
    }

    #pragma acc kernels copy(a[0:1024]) copyout(b[0:1024])
    {
        #pragma acc loop
        for (int _0 = 0; _0 < 2; ++_0){
            #pragma acc loop
            for (int _1 = 0; _1 < 2; ++_1){
                #pragma acc loop
                for (int _2 = 0; _2 < 2; ++_2){
                    #pragma acc loop
                    for (int _3 = 0; _3 < 2; ++_3){
                        #pragma acc loop
                        for (int _4 = 0; _4 < 2; ++_4){
                            #pragma acc loop
                            for (int _5 = 0; _5 < 2; ++_5){
                                #pragma acc loop
                                for (int _6 = 0; _6 < 2; ++_6){
                                    #pragma acc loop
                                    for (int _7 = 0; _7 < 2; ++_7){
                                        #pragma acc loop
                                        for (int _8 = 0; _8 < 2; ++_8){
                                            #pragma acc loop
                                            for (int _9 = 0; _9 < 2; ++_9){
                                                b[_0 * 512 + _1 * 256 + _2 * 128 + _3 * 64 + _4 * 32 + _5 * 16 + _6 * 8 + _7 * 4 + _8 * 2 + _9] = a[_0 * 512 + _1 * 256 + _2 * 128 + _3 * 64 + _4 * 32 + _5 * 16 + _6 * 8 + _7 * 4 + _8 * 2 + _9];
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    for (int x = 0; x < 1024; ++x){
        if (fabs(a[x] - b[x]) > PRECISION){
            err += 1;
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
