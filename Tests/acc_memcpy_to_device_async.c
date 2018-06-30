#include "acc_testsuite.h"

int test(){
    int err = 0;
    real_t *a = (real_t *)malloc(n * sizeof(real_t));
    real_t *b = (real_t *)malloc(n * sizeof(real_t));
    real_t *c = (real_t *)malloc(n * sizeof(real_t));
    real_t *d = (real_t *)malloc(n * sizeof(real_t));
    real_t *e = (real_t *)malloc(n * sizeof(real_t));
    real_t *f = (real_t *)malloc(n * sizeof(real_t));

    real_t *a_host = (real_t *)malloc(n * sizeof(real_t));
    real_t *b_host = (real_t *)malloc(n * sizeof(real_t));
    real_t *d_host = (real_t *)malloc(n * sizeof(real_t));
    real_t *e_host = (real_t *)malloc(n * sizeof(real_t));

    real_t *hostdata = (real_t *)malloc(6 * n * sizeof(real_t));
    real_t *devdata;

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 1;
        d[x] = rand() / (real_t)(RAND_MAX / 10);
        e[x] = rand() / (real_t)(RAND_MAX / 10);
        f[x] = 2;

        a_host[x] = a[x];
        b_host[x] = b[x];
        d_host[x] = d[x];
        e_host[x] = e[x];
    }

    devdata = acc_create(hostdata, 6 * n * sizeof(real_t));

    acc_memcpy_to_device_async(devdata, a, n * sizeof(real_t), 1);
    acc_memcpy_to_device_async(&(devdata[n]), b, n * sizeof(real_t), 2);
    acc_memcpy_to_device_async(&(devdata[2*n]), c, n * sizeof(real_t), 3);
    acc_memcpy_to_device_async(&(devdata[3*n]), d, n * sizeof(real_t), 4);
    acc_memcpy_to_device_async(&(devdata[4*n]), e, n * sizeof(real_t), 5);
    acc_memcpy_to_device_async(&(devdata[5*n]), f, n * sizeof(real_t), 6);

    #pragma acc data deviceptr(devdata)
    {
        #pragma acc parallel async(1)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                devdata[x] = devdata[x] * devdata[x];
            }
        }
        #pragma acc parallel async(2)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                devdata[n + x] = devdata[n + x] * devdata[n + x];
            }
        }
        #pragma acc parallel async(4)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                devdata[3*n + x] = devdata[3*n + x] * devdata[3*n + x];
            }
        }
        #pragma acc parallel async(5)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                devdata[4*n + x] = devdata[4*n + x] * devdata[4*n + x];
            }
        }
        #pragma acc parallel async(3) wait(1, 2)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                devdata[2*n + x] += devdata[x] + devdata[n + x];
            }
        }
        #pragma acc parallel async(6) wait(4, 5)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                devdata[5*n + x] += devdata[3*n + x] + devdata[4*n + x];
            }
        }
    }

    #pragma acc update host(hostdata[2*n:n]) async(3)
    #pragma acc update host(hostdata[5*n:n]) async(6)

    #pragma acc wait(3)
    for (int x = 0; x < n; ++x){
        if (fabs(hostdata[2*n + x] - (1 + a_host[x] * a_host[x] + b_host[x] * b_host[x])) > 4 * PRECISION){
            err += 1;
        }
    }

    #pragma acc wait(6)
    for (int x = 0; x < n; ++x){
        if (fabs(hostdata[5*n + x] - (2 + d_host[x] * d_host[x] + e_host[x] * e_host[x])) > PRECISION){
            err += 1;
        }
    }

    #pragma acc exit data delete(hostdata[0:6*n])

    free(a);
    free(b);
    free(c);
    free(d);
    free(e);
    free(f);
    free(a_host);
    free(b_host);
    free(d_host);
    free(e_host);
    free(hostdata);

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
