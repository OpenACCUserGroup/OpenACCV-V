#include "acc_testsuite.h"
#ifndef T1
//T1:serial,loop,auto,V:2.6-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t * a = new real_t[n];
    real_t * a_copy = new real_t[n];
    real_t * b = new real_t[n];
    real_t rolling_total = 0.0;

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = 0;
    }

    #pragma acc data copyin(a[0:n]) copyout(b[0:n])
    {
      #pragma acc serial loop auto
      for (int x = 0; x < n; ++x){
        b[x] = a[x];
      }
    }

    for (int x = 0; x < n; ++x){
      if (fabs(b[x] - a[x]) > PRECISION){
        err = 1;
      }
    }

    return err;
}
#endif

#ifndef T2
//T2:serial,loop,combined-constructs,V:2.6-2.7
int test2(){
    int err = 0;
    srand(SEED);
    real_t * a = new real_t[n];
    real_t * a_copy = new real_t[n];
    real_t * b = new real_t[n];
    real_t rolling_total = 0.0;

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        a_copy[x] = a[x];
    }

    #pragma acc data copy(a[0:n])
    {
      #pragma acc serial loop auto
      for (int x = 1; x < n; ++x){
        a[x] = a[x - 1] + a[x];
      }
    }

    for (int x = 0; x < n; ++x){
      rolling_total += a_copy[x];
      if (fabs(rolling_total - a[x]) > PRECISION){
        err = 1;
      }
    }

    return err;
}
#endif

int main(){
    int failcode = 0;
    int failed;
#ifndef T1
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x){
        failed = failed + test1();
    }
    if (failed != 0){
        failcode = failcode + (1 << 0);
    }
#endif
#ifndef T2
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x){
        failed = failed + test2();
    }
    if (failed != 0){
        failcode = failcode + (1 << 1);
    }
#endif
    return failcode;
}
