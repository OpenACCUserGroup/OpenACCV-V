#include "acc_testsuite.h"

int test(){
    int err = 0;
    srand(time(NULL));
    int data_on_device = 0;
    int * devtest = (int *)malloc(sizeof(int));
    real_t * a = (real_t *)malloc(n * sizeof(real_t));
    real_t * b = (real_t *)malloc(n * sizeof(real_t));
    devtest[0] = 1;

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = 0;
    }
    
    #pragma acc enter data copyin(devtest[0:1])
    #pragma acc parallel present(devtest[0:1])
    {
        devtest[0] = 0;
    }

    #pragma acc kernels if(data_on_device) present(a[0:n], b[0:n])
    {
        #pragma acc loop
        for (int x = 0; x < n; ++x){
            b[x] = a[x];
        }
    }

    for (int x = 0; x < n; ++x){
        if (fabs(b[x] - a[x]) > PRECISION){
            err += 1;
            printf("1\n");
        }
    }
   
    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = 0;
    }

    #pragma acc enter data copyin(a[0:n]) create(b[0:n])
    data_on_device = 1;

    #pragma acc kernels if(data_on_device) present(a[0:n], b[0:n])
    {
        #pragma acc loop
        for (int x = 0; x < n; ++x){
            b[x] = a[x];
        }
    }
    #pragma acc exit data copyout(b[0:n]) delete(a[0:n])
    
    for (int x = 0; x < n; ++x){
        if (fabs(b[x] - a[x]) > PRECISION){
            err += 1;
            printf("2\n");
        }
    }

    if (devtest[0] == 1){
        for (int x = 0; x < n; ++x){
            a[x] = rand() / (real_t)(RAND_MAX / 10);
            b[x] = 0;
        }
        
        #pragma acc enter data copyin(a[0:n]) create(b[0:n])
        for (int x = 0; x < n; ++x){
            a[x] = -1;
        }

        #pragma acc kernels if(data_on_device) present(a[0:n], b[0:n])
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                b[x] = a[x];
            }
        }
        for (int x = 0; x < n; ++x){
            if (fabs(a[x] + 1) > PRECISION){
                err += 1;
                printf("3\n");
            }
            if (fabs(b[x]) > PRECISION){
                err += 1;
                printf("4\n");
            }
        }
        #pragma acc exit data copyout(a[0:n], b[0:n])
        data_on_device = 0;
        for (int x = 0; x < n; ++x){
            if (fabs(a[x] - b[x]) > PRECISION){
                err += 1;
                printf("5\n");
            }
        }

        for (int x = 0; x < n; ++x){
            a[x] = rand() / (real_t)(RAND_MAX / 10);
            b[x] = 0;
        }
        
        #pragma acc enter data copyin(a[0:n], b[0:n])
        #pragma acc kernels if(data_on_device) present(a[0:n], b[0:n])
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                b[x] = a[x];
            }
        }
        for (int x = 0; x < n; ++x){
            if (fabs(a[x] - b[x]) > PRECISION) {
                err += 1;
                printf("6\n");
            }
        }
        #pragma acc exit data copyout(a[0:n], b[0:n])
        for (int x = 0; x < n; ++x){
            if (fabs(b[x]) > PRECISION && b[x] != a[x]){
                err += 1;
                printf("7\n");
                printf("%f\n", b[x]);
            }
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
