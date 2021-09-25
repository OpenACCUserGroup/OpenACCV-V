#include "acc_testsuite.h"
#ifndef T1
//T1:runtime,construct-independent,V:1.0-2.7
int test1(){
    int err = 0;
    int device_type = acc_get_device_type();

    if (device_type != acc_device_none){
        #pragma acc data copy(err)
        {
            #pragma acc parallel
            {
                if (acc_on_device(device_type) == 0){
                    err += 1;
                }
            }
        }
    }
    else{
        #pragma acc parallel
        {
            if (acc_on_device(acc_device_host) == 0){
                err += 1;
            }
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
