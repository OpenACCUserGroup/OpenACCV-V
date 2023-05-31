#include "acc_testsuite.h"
#ifndef T1
//T1:runtime,data,executable-data,async,construct-independent,reference-counting,V:2.5-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t *a = new real_t[n];
    real_t *b = new real_t[n];
    real_t *c = new real_t[n];
    real_t *d = new real_t[n];
    real_t *e = new real_t[n];
    real_t *f = new real_t[n];

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0;
        d[x] = rand() / (real_t)(RAND_MAX / 10);
        e[x] = rand() / (real_t)(RAND_MAX / 10);
        f[x] = 0;
    }

    #pragma acc enter data create(c[0:n], f[0:n])
    #pragma acc enter data create(c[0:n], f[0:n])

    #pragma acc data copyin(a[0:n], b[0:n], d[0:n], e[0:n]) present(c[0:n], f[0:n])
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
        acc_copyout_finalize_async(c, n * sizeof(real_t), 1);
        acc_copyout_finalize_async(f, n * sizeof(real_t), 2);
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
//T2:runtime,data,executable-data,async,construct-independent,internal-control-values,V:2.5-2.7
int test2(){
    int err = 0;
    srand(SEED);
    real_t * a = new real_t[n];
    real_t * b = new real_t[n];
    real_t * c = new real_t[n];
    int def_async_var = acc_get_default_async();

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0;
    }

    #pragma acc enter data create(c[0:n])
    #pragma acc enter data create(c[0:n])
    
    #pragma acc data copyin(a[0:n], b[0:n])
    {
        #pragma acc parallel present(c[0:n])
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x) {
                c[x] = a[x] + b[x];
            }
        }
        acc_copyout_finalize_async(c, n * sizeof(real_t), def_async_var);
        #pragma acc wait
    }

    for (int x = 0; x < n; ++x) {
        if (fabs(c[x] - (a[x] + b[x])) > PRECISION) {
            err += 1;
        }
    }

    return err;
}
#endif

#ifndef T3
//T3:runtime,data,executable-data,async,construct-independent,internal-control-values,set,V:2.5-2.7
int test3(){
    int err = 0;
    srand(SEED);
    real_t * a = new real_t[n];
    real_t * b = new real_t[n];
    real_t * c = new real_t[n];
    int def_async_var = acc_get_default_async();

    acc_set_default_async(def_async_var + 1);

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0;
    }

    #pragma acc enter data create(c[0:n])
    #pragma acc enter data create(c[0:n])
    #pragma acc data copyin(a[0:n], b[0:n])
    {
        #pragma acc parallel present(c[0:n]) async
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x) {
                c[x] = a[x] + b[x];
            }
        }
        acc_copyout_async(c, n * sizeof(real_t), def_async_var + 1);
        #pragma acc wait
    }

    for (int x = 0; x < n; ++x) {
        if (fabs(c[x] - (a[x] + b[x])) > PRECISION) {
            err += 1;
        }
    }

    return err;
}
#endif

#ifndef T4
//T4:runtime,data,executable-data,async,construct-independent,V:2.5-2.7
int test4(){
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

    #pragma acc enter data create(c[0:n])
    #pragma acc enter data create(c[0:n])
    #pragma acc data copyin(a[0:n], b[0:n])
    {
        #pragma acc parallel present(c[0:n]) async(1)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x) {
                c[x] = a[x] + b[x];
            }
        }
        acc_copyout_finalize_async(c, n * sizeof(real_t), 1);
        #pragma acc enter data copyin(c[0:n]) async(1)
        #pragma acc parallel present(c[0:n]) async(1)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x) {
                c[x] += a[x] + b[x];
            }
        }
    }

    for (int x = 0; x < n; ++x) {
        if (fabs(c[x] - (2 * (a[x] + b[x]))) > PRECISION) {
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
    return failcode;
}
