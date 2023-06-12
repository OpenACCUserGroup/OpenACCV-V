#include "acc_testsuite.h"
#ifndef T1
//T1:async,data,V:3.2-3.3
int test1(){
    int err = 0;
    srand(SEED);
    real_t * a = new real_t[n];
    real_t * b = new real_t[n];
    real_t * c = new real_t[n];

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0.0;
    }

    #pragma acc data copy(a[0:n], b[0:n], c[0:n]) async(1)
    {
        #pragma acc parallel 
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                c[x] += a[x] + b[x];
            }
        }
    }

    #pragma acc data copy(a[0:n], b[0:n], c[0:n]) async(1)
    {
        #pragma acc parallel 
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                c[x] += a[x] + b[x];
            }
        }
    }

    #pragma acc wait(1)
    

    for (int x = 0; x < n; ++x){
        if (fabs(c[x] - (2 * (a[x] + b[x]))) > PRECISION){
            err += 1;
        }
    }

    return err;
}
#endif
#ifndef T2
//T2:async,data,V:3.2-3.3
int test2(){
    int err = 0;
    srand(SEED);
    real_t * a = new real_t[n];
    real_t * b = new real_t[n];
    real_t * c = new real_t[n];

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0.0;
    }

    #pragma acc data copy(a[0:n], b[0:n], c[0:n]) async(0)
    {
        #pragma acc parallel loop async(1) wait(0)
            for (int x = 0; x < n; ++x){
                c[x] += a[x] + b[x];
            }

    #pragma acc wait(1) async(0)
    }

    #pragma acc wait(0)

    for (int x = 0; x < n; ++x){
        if (fabs(c[x] - (a[x] + b[x])) > PRECISION){
            err += 1;
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
