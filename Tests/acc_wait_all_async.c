#include "acc_testsuite.h"

int test(){
    int err = 0;
    real_t *a = (real_t *)malloc(n * sizeof(real_t));
    real_t *b = (real_t *)malloc(n * sizeof(real_t));
    real_t *c = (real_t *)malloc(n * sizeof(real_t));
    real_t *d = (real_t *)malloc(n * sizeof(real_t));
    real_t *e = (real_t *)malloc(n * sizeof(real_t));
    real_t *f = (real_t *)malloc(n * sizeof(real_t));
    real_t *g = (real_t *)malloc(n * sizeof(real_t));
    real_t *h = (real_t *)malloc(n * sizeof(real_t));
    real_t *i = (real_t *)malloc(n * sizeof(real_t));
    real_t *j = (real_t *)malloc(n * sizeof(real_t));
    real_t *k = (real_t *)malloc(n * sizeof(real_t));
    real_t *a_host = (real_t *)malloc(n * sizeof(real_t));
    real_t *d_host = (real_t *)malloc(n * sizeof(real_t));
    real_t *g_host = (real_t *)malloc(n * sizeof(real_t));

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0;
        d[x] = rand() / (real_t)(RAND_MAX / 10);
        e[x] = rand() / (real_t)(RAND_MAX / 10);
        f[x] = 0;
        g[x] = rand() / (real_t)(RAND_MAX / 10);
        h[x] = rand() / (real_t)(RAND_MAX / 10);
        i[x] = 0;
        j[x] = 0;
        k[x] = 0;
        a_host[x] = a[x];
        d_host[x] = d[x];
        g_host[x] = g[x];
    }

    #pragma acc data copyin(a[0:n], b[0:n], d[0:n], e[0:n], g[0:n], h[0:n]) create(c[0:n], f[0:n], i[0:n], j[0:n]) copyout(k[0:n])
    {
        #pragma acc parallel async(1)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                c[x] = a[x] + b[x];
            }
        }
        #pragma acc parallel async(2)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                f[x] = d[x] + e[x];
            }
        }
        #pragma acc parallel async(3)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                i[x] = g[x] + h[x];
            }
        }
        acc_wait_all_async(4);
        #pragma acc parallel async(4)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                j[x] = c[x] + f[x] + i[x];
            }
        }
        #pragma acc parallel async(1)
        {
            for (int x = 0; x < n; ++x){
                a[x] = b[x] * c[x];
            }
        }
        #pragma acc parallel async(2)
        {
            for (int x = 0; x < n; ++x){
                d[x] = e[x] * f[x];
            }
        }
        #pragma acc parallel async(3)
        {
            for (int x = 0; x < n; ++x){
                g[x] = h[x] * i[x];
            }
        }
        acc_wait_all_async(4);
        #pragma acc parallel async(4)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                k[x] = j[x] + a[x] + d[x] + g[x];
            }
        }
        #pragma acc wait(4)
    }

    real_t temp;
    for (int x = 0; x < n; ++x){
        temp = a_host[x] + b[x] + d_host[x] + e[x] + g_host[x] + h[x];
        temp += b[x] * (a_host[x] + b[x]) + e[x] * (d_host[x] + e[x]) + h[x] * (g_host[x] + h[x]);
        if (fabs(k[x] - temp) > PRECISION * 10){
            err += 1;
        }
    }

    free(a);
    free(b);
    free(c);
    free(d);
    free(e);
    free(f);
    free(g);
    free(h);
    free(i);
    free(j);
    free(k);

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
