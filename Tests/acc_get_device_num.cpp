#include "acc_testsuite.h"
#ifndef T1
//T1:runtime,devonly,internal-control-values,syntactic,V:1.0-2.7
int test1(){
    int err = 0;

    if (acc_get_device_type() != acc_device_none){
        for (int x = 0; x < acc_get_num_devices(acc_get_device_type()); ++x){
            acc_set_device_num(x, acc_get_device_type());
            if (acc_get_device_num(acc_get_device_type()) != x){
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
