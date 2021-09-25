#include "acc_testsuite.h"
#ifndef T1
//T1:runtime,async,construct-independent,wait,V:2.0-2.7
int test1(){
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
