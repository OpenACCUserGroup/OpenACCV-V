#include "acc_testsuite.h"
#ifndef T1
//T1:runtime,async,construct-independent,set,V:2.5-2.7
int test1(){
    int err = 0;
    real_t *a = new real_t[n];
    real_t *b = new real_t[n];
    real_t *c = new real_t[n];
    real_t *a_host = new real_t[n];
    real_t *b_host = new real_t[n];

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0;
        a_host[x] = a[x];
        b_host[x] = b[x];
    }

    #pragma acc data copyin(a[0:n], b[0:n]) copyout(c[0:n])
    {
        acc_set_default_async(1);
        #pragma acc parallel async
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                a[x] = a[x] * a[x];
            }
        }
        acc_set_default_async(2);
        #pragma acc parallel async
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                b[x] = b[x] * b[x];
            }
        }
        #pragma acc parallel async(1) wait(2)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                c[x] = a[x] + b[x];
            }
        }
        #pragma acc wait(1)
    }

    for (int x = 0; x < n; ++x){
        if (fabs(c[x] - (a_host[x] * a_host[x] + b_host[x] * b_host[x])) > 4*PRECISION){
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
    return failcode;
}
