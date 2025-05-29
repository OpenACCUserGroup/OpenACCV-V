#include "acc_testsuite.h"
#ifndef T1
//T1:async,runtime,construct-independent,V:2.0-2.7
int test1(){
    int err = 0;
    real_t *a = new real_t[n];
    real_t *b = new real_t[n];
    real_t *c = new real_t[n];
    real_t *d = new real_t[n];
    real_t *e = new real_t[n];

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0;
        d[x] = rand() / (real_t)(RAND_MAX / 10);
        e[x] = 0;
    }

    #pragma acc enter data copyin(a[0:n], b[0:n]) create(c[0:n]) async(1)
    #pragma acc enter data copyin(d[0:n]) create(e[0:n]) async(2)
    #pragma acc parallel present(a[0:n], b[0:n], c[0:n]) async(1)
    {
        #pragma acc loop
        for (int x = 0; x < n; ++x){
            c[x] = a[x] + b[x];
        }
    }
    #pragma acc parallel present(c[0:n], d[0:n], e[0:n]) async(1) wait(2)
    {
        #pragma acc loop
        for (int x = 0; x < n; ++x){
            e[x] = c[x] + d[x];
        }
    }
    #pragma acc exit data copyout(e[0:n]) async(1)

    while (!acc_async_test(1));
    for (int x = 0; x < n; ++x){
        if (fabs(e[x] - (a[x] + b[x] + d[x])) > PRECISION){
            err += 1;
        }
    }

    return err;
}
#endif

#ifndef T2
//T2:async,runtime,construct-independent,V:1.0-2.7
int test2(){
    int err = 0;
    real_t *a = new real_t[n];
    real_t *b = new real_t[n];
    real_t *c = new real_t[n];
    real_t *d = new real_t[n];
    real_t *e = new real_t[n];

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0;
        d[x] = rand() / (real_t)(RAND_MAX / 10);
        e[x] = 0;
    }
    #pragma acc data copyin(a[0:n], b[0:n], d[0:n]) create(c[0:n]) copyout(e[0:n])
    {
        #pragma acc parallel present(a[0:n], b[0:n], c[0:n]) async(1)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x) {
                c[x] = a[x] + b[x];
            }
        }
        #pragma acc parallel present(c[0:n], d[0:n], e[0:n]) async(1)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x) {
                e[x] = c[x] + d[x];
            }
        }
        while (!acc_async_test(1));
    }

    for (int x = 0; x < n; ++x) {
        if (fabs(e[x] - (a[x] + b[x] + d[x])) > PRECISION) {
            err += 1;
        }
    }
    return err;
}
#endif

#ifndef T3
//T3:async,runtime,construct-independent,V:2.5-2.7
int test3() {
    int err = 0;
    real_t* a = new real_t[n];
    real_t* b = new real_t[n];
    real_t* c = new real_t[n];
    real_t* d = new real_t[n];
    real_t* e = new real_t[n];
    int async_val = acc_get_default_async();

    for (int x = 0; x < n; ++x) {
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0;
        d[x] = rand() / (real_t)(RAND_MAX / 10);
        e[x] = 0;
    }
    #pragma acc data copyin(a[0:n], b[0:n], d[0:n]) create(c[0:n]) copyout(e[0:n])
    {
        #pragma acc parallel present(a[0:n], b[0:n], c[0:n]) async
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x) {
                c[x] = a[x] + b[x];
            }
        }
        #pragma acc parallel present(c[0:n], d[0:n], e[0:n]) async
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x) {
                e[x] = c[x] + d[x];
            }
        }
        while (!acc_async_test(async_val));
    }

    for (int x = 0; x < n; ++x) {
        if (fabs(e[x] - (a[x] + b[x] + d[x])) > PRECISION) {
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
    for (int x = 0; x < NUM_TEST_CALLS; ++x) {
        failed = failed + test3();
    }
    if (failed != 0) {
        failcode = failcode + (1 << 2);
    }
#endif
    return failcode;
}
