#include "acc_testsuite.h"
#ifndef T1
//T1:runtime,data,executable-data,devonly,V:2.0-2.7
int test1(){
    int err = 0;
    real_t *a = (real_t *)malloc(n * sizeof(real_t));
    int *devtest = (int *)malloc(sizeof(int));

    devtest[0] = 1;
    #pragma acc enter data copyin(devtest[0:1])
    #pragma acc parallel present(devtest[0:1])
    {
        devtest[0] = 0;
    }

    #pragma acc enter data create(a[0:n])
    if (acc_is_present(a, n * sizeof(real_t)) == 0){
        err += 1;
    }
    #pragma acc exit data delete(a[0:n])

    if (devtest[0] == 1){
        if (acc_is_present(a, n * sizeof(real_t)) != 0){
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
    return failcode;
}
