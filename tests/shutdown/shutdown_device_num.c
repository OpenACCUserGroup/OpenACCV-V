#include "acc_testsuite.h"
#ifndef T1
//T1:shutdown,runtime,syntactic,V:2.5-2.7
int test1(){
    int err = 0;
    srand(SEED);
    int device_num;

    device_num = acc_get_device_num(acc_get_device_type());
    #pragma acc shutdown device_num(device_num)

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
