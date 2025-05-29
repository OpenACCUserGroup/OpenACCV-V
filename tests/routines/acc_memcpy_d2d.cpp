#include "acc_testsuite.h"
#include <stdlib.h>
#include <math.h>

#ifndef T1
//T1:runtime,data,executable-data,construct-independent,V:3.3
int test1(){
    int err = 0;
    real_t *a = new real_t[n];
    real_t *b = new real_t[n];
    real_t *c = new real_t[n];

    acc_set_device_num(0, acc_device_nvidia);
    #pragma acc enter data create(a[0:n])
    acc_set_device_num(1, acc_device_nvidia);
    #pragma acc enter data create(b[0:n])
    

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = a[x];
        c[x] = 4 * a[x];
    }
    acc_set_device_num(0, acc_device_nvidia);
    #pragma acc update device(a[0:n])
    acc_set_device_num(1, acc_device_nvidia);
    #pragma acc update device(b[0:n])
    
    acc_set_device_num(0, acc_device_nvidia);
    #pragma acc data present(a)
    {
        #pragma acc parallel loop 
        for(int i = 0; i < n; ++i){
            a[i] *= 2;
        }
        acc_memcpy_d2d(b, a, n * sizeof(real_t), 1, 0);

    }

    acc_set_device_num(1, acc_device_nvidia);
    #pragma acc parallel loop
    for(int i = 0; i < n; ++i){
        b[i] *= 2;
    }

    #pragma acc update host(b[0:n])

    for(int x = 0; x < n; ++x){
        if(fabs(b[x] - c[x]) > PRECISION){
            err++;
            break;
        }
    }

    #pragma acc exit data delete(a[0:n], b[0:n])

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
