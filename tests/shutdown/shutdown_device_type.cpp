#include "acc_testsuite.h"
#ifndef T1
//T1:shutdown,runtime,syntactic,V:2.5-3.2
int test1(){
    int err = 0;

    #pragma acc shutdown device_type(host)

    return err;
}
#endif
#ifndef T2
//T2:shutdown,runtime,syntactic,V:2.5-3.2
int test2(){
    int err = 0;

    #pragma acc shutdown device_type(multicore)

    return err;
}
#endif
#ifndef T3
//T3:shutdown,runtime,syntactic,V:2.5-3.2
int test3(){
    int err = 0;

    #pragma acc shutdown device_type(default)

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
#ifndef T2
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x){
        failed = failed + test2();
    }
    if (failed != 0){
        failcode = failcode + (1 << 1);
    }
#endif
#ifndef T3
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x){
        failed = failed + test3();
    }
    if (failed != 0){
        failcode = failcode + (1 << 2);
    }
#endif
    return failcode;
}
