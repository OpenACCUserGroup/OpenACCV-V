#include "acc_testsuite.h"
#include <cstdlib>
#include <cmath>

#ifndef T1
//T1:routine,init,runtime,V:3.2-3.3
int test1(){
    int err = 0;

    if (acc_get_device_type() != acc_device_none){
        acc_init_device(1, acc_get_device_type());
    }

    return err;
}
#endif

#ifndef T2
//T2:routine,init,runtime,V:3.2-3.3
int test2(){
    int err = 0;
    real_t* a = new real_t[n];
    real_t* b = new real_t[n];
    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = a[x] * 2;
    }

    if (acc_get_device_type() != acc_device_none){
        acc_init_device(1, acc_get_device_type());
    }

    #pragma acc parallel loop copy(a[0:n])
    for(int x = 0; x < n; x++) {
        a[x] *= 2;
    }

    for (int x = 0; x < n; ++x){
        if (std::fabs(a[x] - b[x]) > PRECISION){
            err += 1;
        }
    }

    delete[] a;
    delete[] b;

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