#include "acc_testsuite.h"
#ifndef T1
//T1:serial,data,executable-data,async,wait,V:2.6-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t *a = new real_t[n];
    real_t *b = new real_t[n];

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = a[x];
    }

    #pragma acc enter data create(a[0:n])
    #pragma acc update device(a[0:n]) async(1)
    #pragma acc serial present(a[0:n]) wait(1)
    {
        #pragma acc loop
        for (int x = 0; x < n; ++x){
            a[x] += 1;
        }
    }
    #pragma acc exit data copyout(a[0:n])
    for (int x = 0; x < n; ++x){
        if (fabs(a[x] - (b[x] + 1)) > PRECISION){
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
