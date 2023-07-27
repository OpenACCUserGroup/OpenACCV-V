#include "acc_testsuite.h"
#ifndef T1
//T1:runtime,data,executable-data,reference-counting,syntactic,construct-independent,async,V:2.5-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t *a = (real_t *)malloc(n * sizeof(real_t));
    real_t *b = (real_t *)malloc(n * sizeof(real_t));
    real_t *c = (real_t *)malloc(n * sizeof(real_t));
    real_t *d = (real_t *)malloc(n * sizeof(real_t));
    real_t *e = (real_t *)malloc(n * sizeof(real_t));
    real_t *f = (real_t *)malloc(n * sizeof(real_t));

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0;
        d[x] = rand() / (real_t)(RAND_MAX / 10);
        e[x] = rand() / (real_t)(RAND_MAX / 10);
        f[x] = 0;
    }

    #pragma acc enter data copyin(a[0:n], b[0:n], d[0:n], e[0:n])
    #pragma acc data present(a[0:n], b[0:n], d[0:n], e[0:n]) copyout(c[0:n], f[0:n])
    {
        #pragma acc parallel async(1)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                c[x] = a[x] + b[x];
            }
        }
        #pragma acc parallel async(2)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                f[x] = d[x] + e[x];
            }
        }
        acc_delete_finalize_async(a, n * sizeof(real_t), 1);
        acc_delete_finalize_async(b, n * sizeof(real_t), 1);
        acc_delete_finalize_async(d, n * sizeof(real_t), 2);
        acc_delete_finalize_async(e, n * sizeof(real_t), 2);
        #pragma acc wait
    }

    for (int x = 0; x < n; ++x){
        if (fabs(c[x] - (a[x] + b[x])) > PRECISION){
            err += 1;
        }
        if (fabs(f[x] - (d[x] + e[x])) > PRECISION){
            err += 1;
        }
    }

    return err;
}
#endif

#ifndef T2
//T2:runtime,data,executable-data,reference-counting,devonly,async,construct-independent,V:2.5-2.7
int test2(){
    int err = 0;
    srand(SEED);
    real_t *a = (real_t *)malloc(n * sizeof(real_t));
    real_t *b = (real_t *)malloc(n * sizeof(real_t));
    real_t *c = (real_t *)malloc(n * sizeof(real_t));
    real_t *d = (real_t *)malloc(n * sizeof(real_t));
    real_t *e = (real_t *)malloc(n * sizeof(real_t));
    real_t *f = (real_t *)malloc(n * sizeof(real_t));
    int* devtest = (int *)malloc(sizeof(int));

    devtest[0] = 1;
    #pragma acc enter data copyin(devtest[0:1])
    #pragma acc parallel present(devtest[0:1])
    {
        devtest[0] = 0;
    }

    if (devtest[0] == 1){
        for (int x = 0; x < n; ++x){
            a[x] = rand() / (real_t)(RAND_MAX / 10);
            b[x] = rand() / (real_t)(RAND_MAX / 10);
            c[x] = 0;
        }

        #pragma acc enter data copyin(a[0:n], b[0:n], c[0:n])
        #pragma acc data present(a[0:n], b[0:n], c[0:n])
        {
            #pragma acc parallel async(1)
            {
                #pragma acc loop
                for (int x = 0; x < n; ++x){
                    c[x] = a[x] * b[x];
                }
            }
        }
        acc_delete_finalize_async(c, n * sizeof(real_t), 1);
        #pragma acc enter data copyin(c[0:n]) async(1)
        #pragma acc data present(a[0:n], b[0:n], c[0:n]) 
        {
            #pragma acc parallel async(1)
            {
                #pragma acc loop
                for (int x = 0; x < n; ++x){
                    c[x] += a[x] + b[x];
                }
            }
        }
        #pragma acc wait
        #pragma acc exit data delete(a[0:n], b[0:n]) copyout(c[0:n])

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
