#include "acc_testsuite.h"
#define DECLARE_TEST
#define DECLARE_CREATE
#include "acc_testsuite_declare.h"
#pragma acc declare create(fixed_size_array)
#pragma acc declare create(scalar)
#pragma acc declare create(datapointer)

int mult_create = 2;

#pragma acc declare create(n)
#pragma acc routine vector
void multiplyData(real_t *a){
    for (int x = 0; x < n; ++x){
        a[x] = a[x] * 2;
    }
}

#ifndef T1
//T1:declare,construct-independent,V:1.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t * a = (real_t *)malloc(n * sizeof(real_t));
    real_t * b = (real_t *)malloc(n * sizeof(real_t));
    int mult = 2;
    #pragma acc update device(n)

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = 0;
    }

    #pragma acc data copyin(a[0:n]) copyout(b[0:n]) present(fixed_size_array)
    {
        #pragma acc parallel
        {
            #pragma acc loop
            for (int x = 0; x < 10; ++x){
                fixed_size_array[x] = x*x;
            }
        }
        #pragma acc parallel
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                b[x] = a[x] + fixed_size_array[x%10];
            }
        }
    }

    for (int x = 0; x < n; ++x){
        if (fabs(b[x] - (a[x] + (x%10) * (x%10))) > PRECISION){
            err += 1;
            break;
        }
    }

    return err;
}
#endif

#ifndef T2
//T2:declare,construct-independent,V:1.0-2.7
int test2(){
    int err = 0;
    srand(SEED);
    real_t * a = (real_t *)malloc(n * sizeof(real_t));
    real_t * b = (real_t *)malloc(n * sizeof(real_t));
    int mult = 2;
    #pragma acc update device(n)

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = 0;
    }

    scalar = 10;
    #pragma acc update device(scalar)
    #pragma acc data copyin(a[0:n]) copyout(b[0:n])
    {
        #pragma acc parallel
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                b[x] = a[x] + scalar;
            }
        }
    }

    for (int x = 0; x < n; ++x){
        if (fabs(b[x] - (a[x] + 10)) > PRECISION){
            err += 1;
            break;
        }
    }

    return err;
}
#endif

#ifndef T3
//T3:declare,construct-independent,V:1.0-2.7
int test3(){
    int err = 0;
    srand(SEED);
    real_t * a = (real_t *)malloc(n * sizeof(real_t));
    real_t * b = (real_t *)malloc(n * sizeof(real_t));
    int mult = 2;
    #pragma acc update device(n)

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = a[x];
    }

    #pragma acc update device(mult_create)
    #pragma acc data copy(a[0:n])
    {
        #pragma acc parallel
        {
            #pragma acc loop
            for (int x = 0; x < 1; ++x){
                extern_multiplyData_create(a, n);
            }
        }
    }

    for (int x = 0; x < n; ++x){
        if (fabs(a[x] - (b[x] * 2)) > PRECISION){
            err += 1;
            break;
        }
    }

    return err;
}
#endif

#ifndef T4
//T4:declare,construct-independent,V:2.0-2.7
int test4(){
    int err = 0;
    srand(SEED);
    real_t * a = (real_t *)malloc(n * sizeof(real_t));
    real_t * b = (real_t *)malloc(n * sizeof(real_t));
    int mult = 2;
    #pragma acc update device(n)

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = a[x];
    }
    #pragma acc data copy(a[0:n])
    {
        #pragma acc parallel
        {
            #pragma acc loop
            for (int x = 0; x < 1; ++x){
                multiplyData(a);
            }
        }
    }

    for (int x = 0; x < n; ++x){
        if (fabs(a[x] - (b[x] * 2)) > PRECISION){
            err += 1;
            break;
        }
    }

    return err;
}
#endif

#ifndef T5
//T5:declare,construct-independent,V:2.6-2.7
int test5(){
    int err = 0;
    srand(SEED);
    real_t * a = (real_t *)malloc(n * sizeof(real_t));
    real_t * b = (real_t *)malloc(n * sizeof(real_t));
    int mult = 2;
    #pragma acc update device(n)

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = a[x];
    }
    datapointer = a;

    #pragma acc data copyin(a[0:n]) attach(datapointer)
    {
        #pragma acc parallel present(datapointer[0:n])
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                datapointer[x] = datapointer[x] * 2;
            }
        }
    }

    #pragma acc exit data copyout(a[0:n])
    for (int x = 0; x < n; ++x){
        if (fabs(a[x] - (b[x] * 2)) > PRECISION){
            err += 1;
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
