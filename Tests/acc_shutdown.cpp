#include "acc_testsuite.h"
#include <cstdlib>
#include <cmath>

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

#ifndef T2
//T2:runtime,devonly,shutdown,compute,V:2.0-2.7
int test2(){
    int err = 0;
    int n = 1000;
    real_t* a = new real_t[n];
    
    for (int i = 0; i < n; i++) {
        a[i] = static_cast<real_t>(i);
    }

    if (acc_get_device_type() != acc_device_none){
        #pragma acc parallel loop copy(a[0:n])
        for (int i = 0; i < n; i++) {
            a[i] = a[i] * 2.0;
        }

        acc_shutdown(acc_get_device_type());
    }

    for (int i = 0; i < n; i++) {
        if (std::abs(a[i] - (static_cast<real_t>(i) * 2.0)) > PRECISION) {
            err++;
        }
    }

    delete[] a;
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
    return failcode;
}