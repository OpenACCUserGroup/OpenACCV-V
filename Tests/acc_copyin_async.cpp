#include "acc_testsuite.h"
#ifndef T1
//T1:async,data,executable-data,runtime,construct-independent,V:2.5-2.7
int test1(){
    int err = 0;
    real_t *a = new real_t[n];
    real_t *b = new real_t[n];
    real_t *c = new real_t[n];
    real_t *a_host = new real_t[n];
    real_t *b_host = new real_t[n];

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0;
        a_host[x] = a[x];
        b_host[x] = b[x];
    }

    acc_copyin_async(a, n * sizeof(real_t), 1);
    acc_copyin_async(b, n * sizeof(real_t), 2);
    #pragma acc data copyout(c[0:n]) present(a[0:n], b[0:n])
    {
        #pragma acc parallel async(1)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                a[x] = a[x] * a[x];
            }
        }
        #pragma acc parallel async(2)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                b[x] = b[x] * b[x];
            }
        }
        #pragma acc parallel async(2) wait(1)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                c[x] = a[x] + b[x];
            }
        }
    }

    for (int x = 0; x < n; ++x){
        if (fabs(c[x] - (a_host[x] * a_host[x] + b_host[x] * b_host[x])) > PRECISION * 4){
            err += 1;
        }
    }

    return err;
}
#endif

#ifndef T2
//T2:runtime,data,executable-data,async,construct-independent,V:2.5-2.7
int test2(){
    int err = 0;
    srand(SEED);
    real_t * a = new real_t[n];
    real_t * b = new real_t[n];
    real_t * c = new real_t[n];
    real_t * a_copy = new real_t[n];
    real_t * b_copy = new real_t[n];

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 1;
    }

    acc_copyin_async(a, n*sizeof(real_t), 1);
    acc_copyin_async(b, n*sizeof(real_t), 2);

    #pragma acc data copyout(c[0:n])
    {
        while (!acc_async_test_all());
        #pragma acc parallel present(a[0:n], b[0:n])
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x) {
                c[x] = a[x] + b[x];
            }
        }
    }

    #pragma acc exit data delete(a[0:n], b[0:n])

    for (int x = 0; x < n; ++x) {
        if (fabs(c[x] - (a[x] + b[x])) > PRECISION) {
            err += 1;
        }
    }

    return err;
}
#endif

#ifndef T3
//T3:runtime,data,executable-data,async,construct-independent,V:2.5-2.7
int test3(){
    int err = 0;
    srand(SEED);
    real_t * a = new real_t[n];
    real_t * b = new real_t[n];
    real_t * c = new real_t[n];

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0;
    }

    acc_copyin_async(a, n * sizeof(real_t), 1);
    acc_copyin_async(b, n * sizeof(real_t), 2);

    #pragma acc data copyout(c[0:n])
    {
        #pragma acc wait
        #pragma acc parallel present(a[0:n], b[0:n])
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x) {
                c[x] = a[x] + b[x];
            }
        }
    }

    #pragma acc exit data delete(a[0:n], b[0:n])

    for (int x = 0; x < n; ++x) {
        if (fabs(c[x] - (a[x] + b[x])) > PRECISION) {
            err = err + 1;
        }
    }

    return err;
}
#endif

#ifndef T4
//T4:runtime,data,executable-data,async,construct-independent,reference-counting,V:2.5-2.7
int test4(){
    int err = 0;
    srand(SEED);
    real_t * a = new real_t[n];
    real_t * b = new real_t[n];
    real_t * c = new real_t[n];
    real_t * d = new real_t[n];
    real_t * e = new real_t[n];

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0;
        d[x] = rand() / (real_t)(RAND_MAX / 10);
        e[x] = 0;
    }

    #pragma acc enter data create(c[0:n])
    #pragma acc data copyin(a[0:n], b[0:n], d[0:n]) copyout(e[0:n])
    {
        #pragma acc parallel present(c[0:n]) async(1)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x) {
                c[x] = a[x] + b[x];
            }
        }
        acc_copyin_async(c, n * sizeof(real_t), 1);
        #pragma acc exit data delete(c[0:n]) async(1)
        #pragma acc parallel present(c[0:n]) async(1)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x) {
                e[x] = c[x] + d[x];
            }
        }
    }
    #pragma acc wait
    #pragma acc exit data copyout(c[0:n])

    for (int x = 0; x < n; ++x) {
        if (fabs(c[x] - (a[x] + b[x])) > PRECISION) {
            err += 1;
        }
        if (fabs(e[x] - (a[x] + b[x] + d[x])) > PRECISION) {
            err += 1;
        }
    }

    return err;
}
#endif

#ifndef T5
//T5:runtime,data,executable-data,async,construct-independent,reference-counting,V:2.5-2.7
int test5(){
    int err = 0;
    srand(SEED);
    real_t * a = new real_t[n];
    real_t * b = new real_t[n];
    real_t * c = new real_t[n];
    real_t * a_copy = new real_t[n];
    real_t * b_copy = new real_t[n];
    int * devtest = (int *)malloc(sizeof(int));

    devtest[0] = 0;
    #pragma acc enter data copyin(devtest[0:1])
    #pragma acc parallel present(devtest[0:1])
    {
        devtest[0] = 1;
    }

    if (devtest[0] == 0){
        for (int x = 0; x < n; ++x){
            a[x] = rand() / (real_t)(RAND_MAX / 10);
            b[x] = rand() / (real_t)(RAND_MAX / 10);
            c[x] = 0;
            a_copy[x] = a[x];
            b_copy[x] = b[x];
        }

        acc_copyin_async(a, n * sizeof(real_t), 1);
        acc_copyin_async(b, n * sizeof(real_t), 2);

        for (int x = 0; x < n; ++x) {
            a[x] = 0;
            b[x] = 0;
        }

        #pragma acc data copyin(a[0:n], b[0:n]) copyout(c[0:n])
        {
            #pragma acc wait
            #pragma acc parallel
            {
                #pragma acc loop
                for (int x = 0; x < n; ++x) {
                    c[x] = a[x] + b[x];
                }
            }
        }

        #pragma acc exit data delete(a[0:n], b[0:n])

        for (int x = 0; x < n; ++x) {
            if (fabs(c[x] - (a_copy[x] + b_copy[x])) > PRECISION) {
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
#ifndef T3
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x){
        failed = failed + test3();
    }
    if (failed != 0){
        failcode = failcode + (1 << 2);
    }
#endif
#ifndef T4
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x){
        failed = failed + test4();
    }
    if (failed != 0){
        failcode = failcode + (1 << 3);
    }
#endif
#ifndef T5
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x){
        failed = failed + test5();
    }
    if (failed != 0){
        failcode = failcode + (1 << 4);
    }
#endif
    return failcode;
}
