#include "acc_testsuite.h"

#ifndef T1
//T1:parallel,loop,reduction,combined-constructs,V:1.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t * a = (real_t *)malloc(n * sizeof(real_t));
    real_t * b = (real_t *)malloc(n * sizeof(real_t));
    real_t min = 1000.0;
    int found = 0;

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX/10);
        b[x] = rand() / (real_t)(RAND_MAX/10);
    }
    #pragma acc data copyin(a[0:n], b[0:n]) copy(min)
    {
        #pragma acc parallel loop reduction(min:min)
        for (int x = 0; x < n; ++x){
            min = fmin(a[x] * b[x], min);
        }
    }

    for (int x = 0; x < n; ++x){
        if (fabs((a[x] * b[x]) - (min + PRECISION)) < 0){
            err = 1;
        }
        if (fabs(min - (a[x] * b[x])) < PRECISION){
            found = 1;
        }
    }
    if (found == 0){
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
    real_t minimums[10];
    real_t host_minimums[10];

    for (int x = 0; x < 10 * n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
    }

    for (int x = 0; x < 10; ++x) {
        minimums[x] = 1000;
        host_minimums[x] = 1000;
    }

    printf("Here");

    #pragma acc data copyin(a[0:10*n], b[0:10*n])
    {
        #pragma acc parallel loop reduction(min:minimums)
        for (int x = 0; x < 10 * n; ++x) {
            minimums[x%10] = fmin(a[x] * b[x], minimums[x%10]);
        }
    }

    for (int x = 0; x < 10 * n; ++x) {
        host_minimums[x%10] = fmin(a[x] * b[x], host_minimums[x%10]);
    }
    for (int x = 0; x < 10; ++x) {
        if (fabs(minimums[x] - host_minimums[x]) > PRECISION) {
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
