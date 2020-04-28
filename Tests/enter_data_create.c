#include "acc_testsuite.h"
#ifndef T1
//T1:data,executable-data,construct-independent,V:2.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t * a = (real_t *)malloc(n * sizeof(real_t));
    real_t * b = (real_t *)malloc(n * sizeof(real_t));
    real_t * c = (real_t *)malloc(n * sizeof(real_t));

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = 0.0;
        c[x] = 0.0;
    }

    #pragma acc enter data create(b[0:n])
    #pragma acc data copyin(a[0:n])
    {
        #pragma acc parallel present(b[0:n])
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                b[x] = a[x];
            }
        }
    }
    #pragma acc data copyout(c[0:n])
    {
        #pragma acc parallel present(b[0:n])
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                c[x] = b[x];
            }
        }
    }
    #pragma acc exit data delete(b[0:n])

    for (int x = 0; x < n; ++x){
        if (fabs(c[x] - a[x]) > PRECISION) {
            err += 1;
            break;
        }
    }

    return err;
}
#endif

#ifndef T2
//T2:data,executable-data,construct-independent,compatibility-features,V:2.0-2.7
int test2(){
    int err = 0;
    srand(SEED);
    real_t * a = (real_t *)malloc(n * sizeof(real_t));
    real_t * b = (real_t *)malloc(n * sizeof(real_t));
    real_t * c = (real_t *)malloc(n * sizeof(real_t));

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = 0.0;
        c[x] = 0.0;
    }

    #pragma acc enter data present_or_create(b[0:n])
    #pragma acc data copyin(a[0:n])
    {
        #pragma acc parallel present(b[0:n])
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                b[x] = a[x];
            }
        }
    }
    #pragma acc data copyout(c[0:n])
    {
        #pragma acc parallel present(b[0:n])
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                c[x] = b[x];
            }
        }
    }
    #pragma acc exit data delete(b[0:n])
    for (int x = 0; x < n; ++x){
        if (fabs(c[x] - a[x]) > PRECISION) {
            err += 2;
            break;
        }
    }

    return err;
}
#endif

#ifndef T3
//T3:data,executable-data,construct-independent,compatibility-features,V:2.0-2.7
int test3(){
    int err = 0;
    srand(SEED);
    real_t * a = (real_t *)malloc(n * sizeof(real_t));
    real_t * b = (real_t *)malloc(n * sizeof(real_t));
    real_t * c = (real_t *)malloc(n * sizeof(real_t));

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = 0.0;
        c[x] = 0.0;
    }

    #pragma acc enter data pcreate(b[0:n])
    #pragma acc data copyin(a[0:n])
    {
        #pragma acc parallel present(b[0:n])
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                b[x] = a[x];
            }
        }
    }
    #pragma acc data copyout(c[0:n])
    {
        #pragma acc parallel present(b[0:n])
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                c[x] = b[x];
            }
        }
    }
    #pragma acc exit data delete(b[0:n])
    for (int x = 0; x < n; ++x){
        if (fabs(c[x] - a[x]) > PRECISION){
            err += 4;
            break;
        }
    }

    return err;
}
#endif

int main(){
    int failcode = 0;
    int testrun;
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
    return failcode;
}
