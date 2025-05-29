#include "acc_testsuite.h"
#ifndef T1
//T1:runtime,data,V:2.0-2.7
int test1(){
    int err = 0;
    real_t *a = (real_t *)malloc(n * sizeof(real_t));
    real_t *a_ptr;

    a_ptr = acc_create(a, n * sizeof(real_t));

    if (a != acc_hostptr(acc_deviceptr(a))){
        err += 1;
    }

    acc_delete(a, n * sizeof(real_t));

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
