#include "acc_testsuite.h"
#ifndef T1
//T1:runtime,data,executable-data,construct-independent,V:2.0-2.7
int test1(){
    int err = 0;
    real_t *a = (real_t *)malloc(n * sizeof(real_t));
    real_t *b = (real_t *)malloc(n * sizeof(real_t));
    real_t *c = (real_t *)malloc(n * sizeof(real_t));
    real_t *hostdata = (real_t *)malloc(3 * n * sizeof(real_t));
    real_t *devdata;

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 1;
    }

    devdata = acc_create(hostdata, 3 * n * sizeof(real_t));

    acc_memcpy_to_device(devdata, a, n * sizeof(real_t));
    acc_memcpy_to_device(&(devdata[n]), b, n * sizeof(real_t));
    acc_memcpy_to_device(&(devdata[2*n]), c, n * sizeof(real_t));

    #pragma acc data deviceptr(devdata)
    {
        #pragma acc parallel
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                devdata[2*n + x] += devdata[x] + devdata[n + x];
            }
        }
    }
    #pragma acc exit data copyout(hostdata[0:3*n])
    for (int x = 0; x < n; ++x){
        if (fabs(hostdata[2 * n + x] - (1 + hostdata[n + x] + hostdata[x])) > PRECISION){
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
