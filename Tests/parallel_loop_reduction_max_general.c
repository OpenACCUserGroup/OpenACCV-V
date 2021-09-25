#include "acc_testsuite.h"
#ifndef T1
//T1:parallel,loop,reduction,combined-constructs,V:1.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t * a = (real_t *)malloc(n * sizeof(real_t));
    real_t * b = (real_t *)malloc(n * sizeof(real_t));
    real_t max = 0.0;
    int found = 0;

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX/10);
        b[x] = rand() / (real_t)(RAND_MAX/10);
    }
    #pragma acc data copyin(a[0:n], b[0:n])
    {
        #pragma acc parallel loop reduction(max:max)
        for (int x = 0; x < n; ++x){
            max = fmax(a[x] * b[x], max);
        }
    }

    for (int x = 0; x < n; ++x){
        if (fabs(max - (a[x] * b[x] + PRECISION)) < 0){
            err = 1;
        }
        if (fabs(max - (a[x] * b[x] - PRECISION)) > 0){
            found = 1;
        }
    }
    if (found = 0){
        err = 1;
    }

    return err;
}
#endif

#ifndef T2
//T2:parallel,reduction,combined-constructs,loop,V:2.7-2.7
int test2(){
    int err = 0;
    srand(SEED);
    real_t * a = (real_t *)malloc(10 * n * sizeof(real_t));
    real_t * b = (real_t *)malloc(10 * n * sizeof(real_t));
    real_t maximums[10];
    real_t max_host[10];

    for (int x = 0; x < 10 * n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
    }

    for (int x = 0; x < 10; ++x) {
        maximums[x] = 0;
        max_host[x] = 0;
    }

    #pragma acc data copyin(a[0:10*n], b[0:10*n])
    {
        #pragma acc parallel loop reduction(max:maximums)
        for (int x = 0; x < 10 * n; ++x) {
            maximums[x%10] = fmax(a[x] * b[x], maximums[x%10]);
        }
    }

    for (int x = 0; x < 10 * n; ++x) {
        max_host[x%10] = fmax(a[x] * b[x], maximums[x%10]);
    }
    
    for (int x = 0; x < 10; ++x) {
        if (fabs(max_host[x] - maximums[x]) > PRECISION) {
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
