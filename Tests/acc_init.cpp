#include "acc_testsuite.h"
#include <cstdlib>
#include <cmath>

#ifndef T1
//T1:runtime,init,syntactic,V:2.5-2.7
int test1(){
    int err = 0;

    if (acc_get_device_type() != acc_device_none){
        acc_init(acc_get_device_type());
    }

    return err;
}
#endif

#ifndef T2
//T2:runtime,init,parallel,V:2.5-2.7
int test2(){
    int err = 0;
    int n = 1000;
    real_t *a = new real_t[n];
    
    for (int i = 0; i < n; i++) {
        a[i] = (real_t)i;
    }

    if (acc_get_device_type() != acc_device_none){
        acc_init(acc_get_device_type());
    }

    #pragma acc parallel loop copy(a[0:n])
    for (int i = 0; i < n; i++) {
        a[i] *= 2.0;
    }

    for (int i = 0; i < n; i++) {
        if (std::fabs(a[i] - (2.0 * i)) > PRECISION) {
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