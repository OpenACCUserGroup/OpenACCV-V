#include "acc_testsuite.h"
#ifndef T1
//T1:serial,async,wait,V:2.6-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t * a = new real_t[n];
    real_t * b = new real_t[n];
    real_t * c = new real_t[n];
    real_t * d = new real_t[n];
    real_t * e = new real_t[n];
    real_t * f = new real_t[n];
    real_t * g = new real_t[n];

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0.0;
        d[x] = rand() / (real_t)(RAND_MAX / 10);
        e[x] = rand() / (real_t)(RAND_MAX / 10);
        f[x] = 0.0;
        g[x] = 0.0;
    }

    #pragma acc data copyin(a[0:n], b[0:n], d[0:n], e[0:n], c[0:n], f[0:n], g[0:n])
    {
        #pragma acc serial async(1)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                c[x] = a[x] + b[x];
            }
        }
        #pragma acc serial async(2)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                f[x] = d[x] + e[x];
            }
        }
        #pragma acc serial wait(1, 2) async(3)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                g[x] = c[x] + f[x];
            }
        }
        #pragma acc update host(c[0:n]) async(1)
        #pragma acc update host(f[0:n]) async(2)
        #pragma acc update host(g[0:n]) async(3)

        #pragma acc wait(1)
        for (int x = 0; x < n; ++x){
            if (fabs(c[x] - (a[x] + b[x])) > PRECISION){
                err += 1;
            }
        }
        #pragma acc wait(2)
        for (int x = 0; x < n; ++x){
            if (fabs(f[x] - (d[x] + e[x])) > PRECISION){
                err += 1;
            }
        }
        #pragma acc wait(3)
        for (int x = 0; x < n; ++x){
            if (fabs(g[x] - (a[x] + b[x] + d[x] + e[x])) > PRECISION){
                err += 1;
            }
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
