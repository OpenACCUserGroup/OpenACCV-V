#include "acc_testsuite.h"
#ifndef T1
//T1:kernels,loop,reduction,combined-constructs,V:1.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t * a = new real_t[n];
    real_t * b = new real_t[n];
    real_t max = 0.0;
    int found = 0;

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX/10);
        b[x] = rand() / (real_t)(RAND_MAX/10);
    }
    #pragma acc data copyin(a[0:n], b[0:n]) copy(max)
    {
        #pragma acc kernels loop reduction(max:max)
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
    if (found == 0){
        err = 1;
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
