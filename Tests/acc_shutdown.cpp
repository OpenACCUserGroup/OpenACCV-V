#include "acc_testsuite.h"
#ifndef T1
//T1:runtime,devonly,shutdown,V:2.0-2.7
int test1(){
    int err = 0;

    if (acc_get_device_type() != acc_device_none){
        acc_shutdown(acc_get_device_type());
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
