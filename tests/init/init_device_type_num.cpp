#include "acc_testsuite.h"
#ifndef T1
//T1:init,runtime,V:2.5-3.2
int test1(){
    int err = 0;
    srand(SEED);
    int device_num = acc_get_device_num(acc_get_device_type());

    #pragma acc init device_type(host) device_num(device_num)

    return err;
}
#endif
#ifndef T2
//T1:init,runtime,V:2.5-3.2
int test2(){
    int err = 0;
    srand(SEED);
    int device_num = acc_get_device_num(acc_get_device_type());

    #pragma acc init device_type(multicore) device_num(device_num)

    return err;
}
#endif
#ifndef T3
//T1:init,runtime,V:2.5-3.2
int test3(){
    int err = 0;
    srand(SEED);
    int device_num = acc_get_device_num(acc_get_device_type());

    #pragma acc init device_type(default) device_num(device_num)

    return err;
}
#endif

int main(){
    int failcode = 0;
    int failed;
#ifndef T1
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x){
        failed += test1();
    }
    if (failed){
        failcode += (1 << 0);
    }
#endif
#ifndef T2
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x){
        failed += test2();
    }
    if (failed){
        failcode += (1 << 1);
    }
#endif
#ifndef T3
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x){
        failed += test3();
    }
    if (failed){
        failcode += (1 << 2);
    }
#endif

    return failcode;
}
