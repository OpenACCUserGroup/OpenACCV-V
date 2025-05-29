#include "acc_testsuite.h"
#ifndef T1
//T1:runtime,data,executable-data,construct-independent,V:2.0-2.7
int test1(){
    int err = 0;
    real_t *a = new real_t[n];
    real_t *b = new real_t[n];
    real_t *c = new real_t[n];
    real_t *a_ptr;
    real_t *b_ptr;
    real_t *c_ptr;

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0;
    }

    #pragma acc enter data copyin(a[0:n], b[0:n]) create(c[0:n])

    a_ptr = reinterpret_cast<real_t*>(acc_deviceptr(a));
    b_ptr = reinterpret_cast<real_t*>(acc_deviceptr(b));
    c_ptr = reinterpret_cast<real_t*>(acc_deviceptr(c));

    #pragma acc data deviceptr(a_ptr, b_ptr, c_ptr)
    {
        #pragma acc parallel
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                c_ptr[x] = a_ptr[x] + b_ptr[x];
            }
        }
    }

    #pragma acc exit data copyout(c[0:n]) delete(a[0:n], b[0:n])

    for (int x = 0; x < n; ++x){
        if (fabs(c[x] - (a[x] + b[x])) > PRECISION){
            err += 1;
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
