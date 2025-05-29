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

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0;
        d[x] = rand() / (real_t)(RAND_MAX / 10);
        e[x] = 0;
    }

    #pragma acc enter data copyin(a[0:n], b[0:n]) create(c[0:n]) async(1)
    #pragma acc enter data copyin(d[0:n]) create(e[0:n]) async(2)
    #pragma acc parallel present(a[0:n], b[0:n], c[0:n]) async(1)
    {
        #pragma acc loop
        for (int x = 0; x < n; ++x){
            c[x] = a[x] + b[x];
        }
    }
    #pragma acc parallel present(c[0:n], d[0:n], e[0:n]) async(1) wait(2)
    {
        #pragma acc loop
        for (int x = 0; x < n; ++x){
            e[x] = c[x] + d[x];
        }
    }
    #pragma acc exit data copyout(e[0:n]) async(1)

    acc_wait(1);
    for (int x = 0; x < n; ++x){
        if (fabs(e[x] - (a[x] + b[x] + d[x])) > PRECISION){
            err += 1;
        }
    }

    #pragma acc exit data delete(a[0:n], b[0:n], c[0:n], d[0:n])

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
