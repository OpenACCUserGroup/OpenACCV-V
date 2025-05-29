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

    real_t *a_host = (real_t *)malloc(n * sizeof(real_t));
    real_t *b_host = (real_t *)malloc(n * sizeof(real_t));
    real_t *d_host = (real_t *)malloc(n * sizeof(real_t));
    real_t *e_host = (real_t *)malloc(n * sizeof(real_t));

    real_t *hostdata = (real_t *)malloc(6 * n * sizeof(real_t));
    real_t *devdata;

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 1;
        d[x] = rand() / (real_t)(RAND_MAX / 10);
        e[x] = rand() / (real_t)(RAND_MAX / 10);
        f[x] = 2;

        a_host[x] = a[x];
        b_host[x] = b[x];
        d_host[x] = d[x];
        e_host[x] = e[x];
    }

    devdata = acc_create(hostdata, 6 * n * sizeof(real_t));

    acc_memcpy_to_device_async(devdata, a, n * sizeof(real_t), 1);
    acc_memcpy_to_device_async(&(devdata[n]), b, n * sizeof(real_t), 2);
    acc_memcpy_to_device_async(&(devdata[2*n]), c, n * sizeof(real_t), 3);
    acc_memcpy_to_device_async(&(devdata[3*n]), d, n * sizeof(real_t), 4);
    acc_memcpy_to_device_async(&(devdata[4*n]), e, n * sizeof(real_t), 5);
    acc_memcpy_to_device_async(&(devdata[5*n]), f, n * sizeof(real_t), 6);

    #pragma acc data deviceptr(devdata)
    {
        #pragma acc parallel async(1)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                devdata[x] = devdata[x] * devdata[x];
            }
        }
        #pragma acc parallel async(2)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                devdata[n + x] = devdata[n + x] * devdata[n + x];
            }
        }
        #pragma acc parallel async(4)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                devdata[3*n + x] = devdata[3*n + x] * devdata[3*n + x];
            }
        }
        #pragma acc parallel async(5)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                devdata[4*n + x] = devdata[4*n + x] * devdata[4*n + x];
            }
        }
        #pragma acc parallel async(3) wait(1, 2)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                devdata[2*n + x] += devdata[x] + devdata[n + x];
            }
        }
        #pragma acc parallel async(6) wait(4, 5)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                devdata[5*n + x] += devdata[3*n + x] + devdata[4*n + x];
            }
        }
    }

    #pragma acc update host(hostdata[2*n:n]) async(3)
    #pragma acc update host(hostdata[5*n:n]) async(6)

    #pragma acc wait(3)
    for (int x = 0; x < n; ++x){
        if (fabs(hostdata[2*n + x] - (1 + a_host[x] * a_host[x] + b_host[x] * b_host[x])) > 4 * PRECISION){
            err += 1;
        }
    }

    #pragma acc wait(6)
    for (int x = 0; x < n; ++x){
        if (fabs(hostdata[5*n + x] - (2 + d_host[x] * d_host[x] + e_host[x] * e_host[x])) > PRECISION){
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
