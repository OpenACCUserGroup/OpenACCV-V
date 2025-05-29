#include "acc_testsuite.h"
#ifndef T1
//T1:init,runtime,V:2.5-3.2
int test1(){
    int err = 0;
    srand(SEED);

    #pragma acc init device_type(nvidia)

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
    return failcode;
}
