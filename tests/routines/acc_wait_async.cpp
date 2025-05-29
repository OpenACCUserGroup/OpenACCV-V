#include "acc_testsuite.h"
#ifndef T1
//T1:runtime,async,construct-independent,wait,V:2.0-2.7
int test1(){
    int err = 0;
    real_t *a = new real_t[n];
    real_t *b = new real_t[n];
    real_t *c = new real_t[n];
    real_t *d = new real_t[n];
    real_t *e = new real_t[n];
    real_t *f = new real_t[n];
    real_t *g = new real_t[n];
    real_t *h = new real_t[n];
    real_t *i = new real_t[n];

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0;
        d[x] = rand() / (real_t)(RAND_MAX / 10);
        e[x] = rand() / (real_t)(RAND_MAX / 10);
        f[x] = 0;
        g[x] = rand() / (real_t)(RAND_MAX / 10);
        h[x] = 0;
        i[x] = 0;
    }

    #pragma acc data copyin(a[0:n], b[0:n], d[0:n], e[0:n], g[0:n]) create(c[0:n], f[0:n], h[0:n]) copyout(i[0:n])
    {
        #pragma acc parallel async(1)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                c[x] = a[x] + b[x];
            }
        }
        acc_wait_async(1, 2);
        #pragma acc parallel async(2)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                h[x] = c[x] + g[x];
            }
        }
        #pragma acc parallel async(1)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                f[x] = d[x] + e[x];
            }
        }
        acc_wait_async(1, 2);
        #pragma acc parallel async(2)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                i[x] = h[x] + f[x];
            }
        }
        #pragma acc wait(2)
    }

    for (int x = 0; x < n; ++x){
        if (fabs(i[x] - (a[x] + b[x] + g[x] + d[x] + e[x])) > PRECISION){
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
