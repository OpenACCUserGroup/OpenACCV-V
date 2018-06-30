#include "acc_testsuite.h"

int test(){
    int err = 0;
    real_t *a = (real_t *)malloc(n * sizeof(real_t));
    real_t *b = (real_t *)malloc(n * sizeof(real_t));
    real_t *c = (real_t *)malloc(n * sizeof(real_t));
    real_t *hostdata = (real_t *)malloc(3 * n * sizeof(real_t));
    real_t *devdata;

    for (int x = 0; x < n; ++x){
        hostdata[x] = rand() / (real_t)(RAND_MAX / 10);
        hostdata[n + x] = rand() / (real_t)(RAND_MAX / 10);
        hostdata[2*n + x] = 1;
    }

    devdata = acc_copyin(hostdata, 3 * n * sizeof(real_t));

    #pragma acc data deviceptr(devdata)
    {
        #pragma acc parallel
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                devdata[2*n + x] += devdata[x] + devdata[n + x];
            }
        }
    }

    acc_memcpy_from_device(a, devdata, n * sizeof(real_t));
    acc_memcpy_from_device(b, &(devdata[n]), n * sizeof(real_t));
    acc_memcpy_from_device(c, &(devdata[2*n]), n * sizeof(real_t));


    for (int x = 0; x < n; ++x){
        if (fabs(a[x] - hostdata[x]) > PRECISION){
            err += 1;
        }
        if (fabs(b[x] - hostdata[n + x]) > PRECISION){
            err += 1;
        }
        if (fabs(c[x] - (1 + a[x] + b[x])) > PRECISION){
            err += 1;
        }
    }

    #pragma acc exit data delete(hostdata[0:3*n])

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
