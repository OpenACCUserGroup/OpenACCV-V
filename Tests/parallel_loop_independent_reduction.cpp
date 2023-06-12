#include "acc_testsuite.h"
#ifndef T1
//T1:parallel,reduction,V:2.7-3.2
int test1(){
    int err = 0;
    srand(SEED);
    real_t * a = new real_t[n];
    real_t reduction;

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
    }

    #pragma acc parallel copyin(a[0:n]) reduction(+:reduction)
    {
        #pragma acc loop independent
        for (int x = 0; x < n; ++x){
            reduction = reduction + a[x];
        }
    }

    for (int x = 0; x < n; ++x){
        reduction = reduction - a[x];
    }
    if (fabs(reduction) > PRECISION){
        err += 1;
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

