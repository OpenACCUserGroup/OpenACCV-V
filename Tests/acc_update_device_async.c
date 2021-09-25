#include "acc_testsuite.h"
#ifndef T1
//T1:runtime,data,executable-data,async,construct-independent,V:2.5-2.7
int test1(){
    int err = 0;
    real_t *a = (real_t *)malloc(n * sizeof(real_t));
    real_t *b = (real_t *)malloc(n * sizeof(real_t));
    real_t *c = (real_t *)malloc(n * sizeof(real_t));
    real_t *a_host = (real_t *)malloc(n * sizeof(real_t));
    real_t *b_host = (real_t *)malloc(n * sizeof(real_t));

    int *devtest = (int *)malloc(sizeof(int));

    devtest[0] = 1;
    #pragma acc enter data copyin(devtest[0:1])
    #pragma acc parallel
    {
        devtest[0] = 0;
    }

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0;
        a_host[x] = a[x];
        b_host[x] = b[x];
    }



    #pragma acc data copyout(c[0:n]) create(a[0:n], b[0:n])
    {
        acc_update_device_async(a, n * sizeof(real_t), 1);
        #pragma acc parallel async(1)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                a[x] = a[x] * a[x];
            }
        }
        acc_update_device_async(b, n * sizeof(real_t), 2);
        #pragma acc parallel async(2)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                b[x] = b[x] * b[x];
            }
        }
        #pragma acc parallel async(1) wait(2)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                c[x] = a[x] + b[x];
            }
        }
        #pragma acc wait(1)
    }

    for (int x = 0; x < n; ++x){
        if (fabs(c[x] - (a_host[x] * a_host[x] + b_host[x] * b_host[x])) > PRECISION){
            err += 1;
        }
    }

    return err;
}
#endif

#ifndef T2
//T2:runtime,data,executable-data,async,devonly,construct-independent,V:2.5-2.7
int test2(){
    int err = 0;
    real_t *a = (real_t *)malloc(n * sizeof(real_t));
    real_t *b = (real_t *)malloc(n * sizeof(real_t));
    real_t *c = (real_t *)malloc(n * sizeof(real_t));
    real_t *a_host = (real_t *)malloc(n * sizeof(real_t));
    real_t *b_host = (real_t *)malloc(n * sizeof(real_t));

    int *devtest = (int *)malloc(sizeof(int));

    devtest[0] = 1;
    #pragma acc enter data copyin(devtest[0:1])
    #pragma acc parallel
    {
        devtest[0] = 0;
    }

    if (devtest[0] == 1){
        for (int x = 0; x < n; ++x){
            a[x] = rand() / (real_t)(RAND_MAX / 10);
            b[x] = rand() / (real_t)(RAND_MAX / 10);
            c[x] = 0;
        }

        #pragma acc data copyout(c[0:n]) copyin(a[0:n], b[0:n])
        {
            #pragma acc parallel async(1)
            {
                #pragma acc loop
                for (int x = 0; x < n; ++x){
                    a[x] = a[x] * a[x];
                }
            }
            acc_update_device_async(a, n * sizeof(real_t), 1);
            #pragma acc parallel async(2)
            {
                #pragma acc loop
                for (int x = 0; x < n; ++x){
                    b[x] = b[x] * b[x];
                }
            }
            acc_update_device_async(b, n * sizeof(real_t), 2);
            #pragma acc parallel async(1) wait(2)
            {
                #pragma acc loop
                for (int x = 0; x < n; ++x){
                    c[x] = a[x] + b[x];
                }
            }
            #pragma acc wait(1)
        }


        for (int x = 0; x < n; ++x){
            if (fabs(c[x] - (a[x] + b[x])) > PRECISION){
                err += 1;
            }
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
