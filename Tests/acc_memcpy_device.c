#include "acc_testsuite.h"
#ifndef T1
//T1:runtime,data,executable-data,construct-independent,V:2.5-2.7
int test1(){
    int err = 0;
    real_t *a = (real_t *)malloc(n * sizeof(real_t));
    real_t *b = (real_t *)malloc(n * sizeof(real_t));
    real_t *c = (real_t *)malloc(n * sizeof(real_t));
    real_t *hostdata = (real_t *)malloc(3 * n * sizeof(real_t));
    real_t *devdata;

    for (int x = 0; x < n; ++x){
        hostdata[x] = rand() / (real_t)(RAND_MAX / 10);
        hostdata[n + x] = rand() / (real_t)(RAND_MAX / 10);
        hostdata[2*n + x] = 1;
    }

    devdata = acc_copyin(hostdata, 3 * n * sizeof(real_t));
    #pragma acc enter data create(a[0:n], b[0:n], c[0:n])
    acc_memcpy_device(acc_deviceptr(a), devdata, n * sizeof(real_t));
    acc_memcpy_device(acc_deviceptr(b), devdata+n, n * sizeof(real_t));
    acc_memcpy_device(acc_deviceptr(c), devdata+2*n, n * sizeof(real_t));

    #pragma acc data present(a[0:n], b[0:n], c[0:n])
    {
        #pragma acc parallel
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                c[x] += a[x] + b[x];
            }
        }
    }

    #pragma acc exit data copyout(a[0:n], b[0:n], c[0:n])

    for (int x = 0; x < n; ++x){
        if (fabs(a[x] - hostdata[x]) > PRECISION){
            err += 1;
        }
        if (fabs(b[x] - hostdata[n + x]) > PRECISION){
            err += 1;
        }
        if (fabs(c[x] - (1 + a[x] + b[x])) > PRECISION){
            err += 1;
        }
    }

    #pragma acc exit data delete(hostdata[0:3*n])

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
