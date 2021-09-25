#include "acc_testsuite.h"
#ifndef T1
//T1:runtime,data,executable-data,async,construct-independent,V:2.5-2.7
int test1(){
    int err = 0;
    real_t *a = (real_t *)malloc(n * sizeof(real_t));
    real_t *b = (real_t *)malloc(n * sizeof(real_t));
    real_t *c = (real_t *)malloc(n * sizeof(real_t));
    real_t *d = (real_t *)malloc(n * sizeof(real_t));
    real_t *e = (real_t *)malloc(n * sizeof(real_t));
    real_t *f = (real_t *)malloc(n * sizeof(real_t));
    real_t *hostdata = (real_t *)malloc(6 * n * sizeof(real_t));
    real_t *hostdata_copy = (real_t *)malloc(6 * n * sizeof(real_t));

    real_t *devdata;

    for (int x = 0; x < n; ++x){
        hostdata[x] = rand() / (real_t)(RAND_MAX / 10);
        hostdata[n + x] = rand() / (real_t)(RAND_MAX / 10);
        hostdata[2*n + x] = 1;
        hostdata[3*n + x] = rand() / (real_t)(RAND_MAX / 10);
        hostdata[4*n + x] = rand() / (real_t)(RAND_MAX / 10);
        hostdata[5*n + x] = 2;
    }

    for (int x = 0; x < 6*n; ++x){
        hostdata_copy[x] = hostdata[x];
    }

    devdata = acc_copyin(hostdata, 6 * n * sizeof(real_t));

    #pragma acc data deviceptr(devdata)
    {
        #pragma acc parallel async(1)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                devdata[x] = devdata[x] * devdata[x];
            }
        }
        acc_memcpy_from_device_async(a, devdata, n * sizeof(real_t), 1);
        #pragma acc parallel async(2)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                devdata[n + x] = devdata[n + x] * devdata[n + x];
            }
        }
        acc_memcpy_from_device_async(b, &(devdata[n]), n * sizeof(real_t), 2);
        #pragma acc parallel async(4)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                devdata[3*n + x] = devdata[3*n + x] * devdata[3*n + x];
            }
        }
        acc_memcpy_from_device_async(d, &(devdata[3*n]), n * sizeof(real_t), 4);
        #pragma acc parallel async(5)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                devdata[4*n + x] = devdata[4*n + x] * devdata[4*n + x];
            }
        }
        acc_memcpy_from_device_async(e, &(devdata[4*n]), n * sizeof(real_t), 5);
        #pragma acc parallel async(3) wait(1, 2)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                devdata[2*n + x] += devdata[x] + devdata[n + x];
            }
        }
        acc_memcpy_from_device_async(c, &(devdata[2*n]), n * sizeof(real_t), 3);
        #pragma acc parallel async(6) wait(4, 5)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                devdata[5*n + x] += devdata[3*n + x] + devdata[4*n + x];
            }
        }
        acc_memcpy_from_device_async(f, &(devdata[5*n]), n * sizeof(real_t), 6);
    }

    #pragma acc wait(1)
    for (int x = 0; x < n; ++x){
        if (fabs(a[x] - hostdata_copy[x] * hostdata_copy[x]) > PRECISION){
            err += 1;
        }
    }
    #pragma acc wait(2)
    for (int x = 0; x < n; ++x){
        if (fabs(b[x] - hostdata_copy[n + x] * hostdata_copy[n + x]) > PRECISION){
            err += 1;
        }
    }
    #pragma acc wait(4)
    for (int x = 0; x < n; ++x){
        if (fabs(d[x] - hostdata_copy[3*n + x] * hostdata_copy[3*n + x]) > PRECISION){
            err += 1;
        }
    }
    #pragma acc wait(5)
    for (int x = 0; x < n; ++x){
        if (fabs(e[x] - hostdata_copy[4*n + x] * hostdata_copy[4*n + x]) > PRECISION){
            err += 1;
        }
    }
    #pragma acc wait(3)
    for (int x = 0; x < n; ++x){
        if (fabs(c[x] - (1 + a[x] + b[x])) > PRECISION){
            err += 1;
        }
    }
    #pragma acc wait(6)
    for (int x = 0; x < n; ++x){
        if (fabs(f[x] - (2 + d[x] + e[x])) > PRECISION){
            err += 1;
        }
    }

    #pragma acc exit data delete(hostdata[0:6*n])

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
