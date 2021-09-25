#include "acc_testsuite.h"
#ifndef T1
//T1:parallel,default,data,data-region,V:2.5-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t * a = (real_t *)malloc(n * sizeof(real_t));

    for (int x = 0; x < n; ++x){
        a[x] = 0.0;
    }
    #pragma acc enter data copyin(a[0:n])
    #pragma acc parallel default(present)
    {
        #pragma acc loop
        for (int x = 0; x < n; ++x){
            a[x] = 1.0;
        }
    }
    #pragma acc exit data copyout(a[0:n])

    for (int x = 0; x < n; ++x){
        if (fabs(a[x] - 1.0) > PRECISION){
            err = 1;
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
