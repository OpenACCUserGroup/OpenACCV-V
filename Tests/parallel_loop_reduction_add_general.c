#include "acc_testsuite.h"
#ifndef T1
//T1:parallel,loop,reduction,combined-constructs,V:1.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t * a = (real_t *)malloc(n * sizeof(real_t));
    real_t * b = (real_t *)malloc(n * sizeof(real_t));
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

int main(){
    int failcode = 0;
    int testrun;
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
