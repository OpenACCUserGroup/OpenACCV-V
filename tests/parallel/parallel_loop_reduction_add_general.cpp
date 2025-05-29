#include "acc_testsuite.h"
#ifndef T1
//T1:parallel,loop,reduction,combined-constructs,V:1.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t * a = new real_t[n];
    real_t * b = new real_t[n];
    real_t total;

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
    }

    total = 10;
    #pragma acc data copyin(a[0:n], b[0:n]) copy(total)
    {
        #pragma acc parallel loop reduction(+:total)
        for (int x = 0; x < n; ++x){
            total += a[x] + b[x];
        }
    }
    for (int x = 0; x < n; ++x){
        total -= a[x] + b[x];
    }
    if (fabs(total - 10) > PRECISION * (2 * n - 1)){ //199 = # of mathematical operations necessary
        err = 1;
    }

    return err;
}
#endif

#ifndef T2
//T2:parallel,loop,reduction,combined-constructs,V:2.7-2.7
int test2(){
    int err = 0;
    srand(SEED);
    real_t * a = new real_t[n * 10];
    real_t * b = new real_t[n * 10];
    real_t c[10];
    real_t c_host[10];

    for (int x = 0; x < 10 * n; ++x) {
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
    }

    for (int x = 0; x < 10; ++x) {
        c[x] = 1.0;
        c_host[x] = 1.0;
    }

    #pragma acc data copyin(a[0:10*n], b[0:10*n])
    {
        #pragma acc parallel loop reduction(+:c[0:10])
        for (int x = 0; x < n; ++x) {
            for (int y = 0; y < 10; ++y) {
                c[y] += a[x * 10 + y] + b[x * 10 + y];
            }
        }
    }

    for (int x = 0; x < n; ++x) {
        for (int y = 0; y < 10; ++y) {
            c_host[y] += a[x * 10 + y] + b[x * 10 + y];
        }
    }

    for (int x = 0; x < 10; ++x) {
        if (fabs(c_host[x] - c[x]) > PRECISION) {
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
