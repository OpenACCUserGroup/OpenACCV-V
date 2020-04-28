#include "acc_testsuite.h"
#pragma acc routine (fmin) seq

#ifndef T1
//T1:serial,loop,reduction,combined-constructs,V:2.6-2.7
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
        #pragma acc serial loop reduction(min:min)
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
