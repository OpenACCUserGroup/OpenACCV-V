#include "acc_testsuite.h"
#ifndef T1
//T1:parallel,loop,combined-constructs,V:1.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t * a = new real_t[n];
    real_t * b = new real_t[n];
    real_t * c = new real_t[n];

    for (int x = 0; x < n; ++x){
        a[x] = 0;
    }

    #pragma acc data copy(a[0:n])
    {
        #pragma acc parallel loop
        for (int x = 0; x < n; ++x){
            a[x] = 1.0;
        }
    }

    for (int x = 0; x < n; ++x){
        if (fabs(a[x] - 1.0) > PRECISION){
            err = 1;
        }
    }

    return err;
}
#endif

#ifndef T2
//T2:parallel,loop,V:1.0-2.7
int test2(){
    int err = 0;
    srand(SEED);
    real_t * a = new real_t[n];
    real_t * b = new real_t[n];
    real_t * c = new real_t[n];

    for (int x = 0; x < n; ++x){
      a[x] = 0.0;
      b[x] = rand() / (real_t)(RAND_MAX / 10);
      c[x] = rand() / (real_t)(RAND_MAX / 10);
    }
    #pragma acc data copy(a[0:n], b[0:n], c[0:n])
    {
        #pragma acc parallel
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                a[x] = b[x];
            }
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                a[x] += c[x];
            }
        }
    }

    for (int x = 0; x < n; ++x){
        if (fabs(a[x] - (b[x] + c[x])) > PRECISION * 2){
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
