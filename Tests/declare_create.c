#define DECLARE_CREATE 1
#include "acc_testsuite_declare.h"
real_t scalar = 2;
real_t* a;

#pragma acc declare create(scalar, a[0:n], n)
#pragma acc update device(n)

void multiply_scalar(real_t* a){
    #pragma acc parallel loop present(a[0:n])
    for(int x = 0; x < n; ++x){
        a[x] += 1;
    }
}

#pragma acc routine vector
void multiply_scalar_routine(real_t* a){
    #pragma acc loop vector
    for(int x = 0; x < n; ++x){
        a[x] += 1;
    }
    #pragma acc update host(a[0:n])
}


#ifndef T1
//T1:declare,construct-independent,V:1.0-3.3
int test1(){
    int err = 0;
    srand(SEED);
    a = (real_t *)malloc(n * sizeof(real_t));
    real_t * b = (real_t *)malloc(n * sizeof(real_t));
    real_t * c = (real_t *)malloc(n * sizeof(real_t));

    #pragma acc enter data create(a[0:n])

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = a[x] * 2;
    }
    #pragma acc update device(a[0:n])

    #pragma acc data copy(c[0:n]) present(a[0:n]) 
    {
        #pragma acc parallel
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                c[x] = a[x] * 2;
            }
        }
    }

    for (int x = 0; x < n; ++x){
        if (fabs(b[x] - c[x]) > PRECISION){
            err += 1;
            break;
        }
    }

    free(b);
    free(c);

    return err;
}
#endif

#ifndef T2
//T2:declare,construct-independent,V:1.0-3.3
int test2(){
    int err = 0;
    srand(SEED);
    real_t* local_a = (real_t *)malloc(n * sizeof(real_t));
    real_t * b = (real_t *)malloc(n * sizeof(real_t));
    real_t * c = (real_t *)malloc(n * sizeof(real_t));

    for (int x = 0; x < n; ++x){
        local_a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = local_a[x] * scalar;
    }

    #pragma acc update device(scalar)

    #pragma acc data copy(local_a[0:n], c[0:n]) present(scalar) 
    {
        #pragma acc parallel
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                c[x] = local_a[x] * scalar;
            }
        }
    }

    for (int x = 0; x < n; ++x){
        if (fabs(b[x] - c[x]) > PRECISION){
            err += 1;
            break;
        }
    }

    free(local_a);
    free(b);
    free(c);

    return err;
}
#endif

#ifndef T3
//T3:declare,construct-independent,V:1.0-3.3
int test3(){
    int err = 0;
    srand(SEED);
    a = (real_t *)malloc(n * sizeof(real_t));
    real_t * b = (real_t *)malloc(n * sizeof(real_t));

    #pragma acc enter data create(a[0:n])

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = a[x] + 1;
    }

    #pragma acc update device(a[0:n])

    multiply_scalar(a);

    #pragma acc update host(a[0:n])

    for (int x = 0; x < n; ++x){
        if (fabs(b[x] - a[x]) > PRECISION){
            err += 1;
            break;
        }
    }

    free(b);

    return err;
}
#endif


#ifndef T4
//T4:declare,construct-independent,V:1.0-3.3
int test4(){
    int err = 0;
    srand(SEED);
    a = (real_t *)malloc(n * sizeof(real_t));
    real_t * b = (real_t *)malloc(n * sizeof(real_t));
    real_t * c = (real_t *)malloc(n * sizeof(real_t));

    #pragma acc enter data create(a[0:n])

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = a[x] * 2;
    }
    #pragma acc update device(a[0:n])

    extern_multiplyData(a);

    for (int x = 0; x < n; ++x){
        if (fabs(b[x] - a[x]) > PRECISION){
            err += 1;
            break;
        }
    }

    free(b);
    free(c);

    return err;
}
#endif

#ifndef T5
//T5:declare,construct-independent,V:1.0-3.3
int test5(){
    int err = 0;
    srand(SEED);
    a = (real_t *)malloc(n * sizeof(real_t));
    real_t * b = (real_t *)malloc(n * sizeof(real_t));
    real_t * c = (real_t *)malloc(n * sizeof(real_t));

    #pragma acc enter data create(a[0:n])

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = a[x] * mult_create;
    }
    #pragma acc update device(a[0:n], mult_create)

    #pragma acc data copy(c[0:n]) present(a[0:n], mult_create)
    {
        #pragma acc parallel
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                c[x] = a[x] * mult_create;
            }
        }
    }

    for (int x = 0; x < n; ++x){
        if (fabs(b[x] - c[x]) > PRECISION){
            err += 1;
            break;
        }
    }

    free(b);
    free(c);

    return err;
}
#endif

#ifndef T6
//T6:declare,construct-independent,V:1.0-3.3
int test6(){
    int err = 0;
    srand(SEED);
    a = (real_t *)malloc(n * sizeof(real_t));
    real_t * b = (real_t *)malloc(n * sizeof(real_t));

    #pragma acc enter data create(a[0:n])

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = a[x] + 1;
    }

    #pragma acc update device(a[0:n])

    multiply_scalar_routine(a);

    for (int x = 0; x < n; ++x){
        if (fabs(b[x] - a[x]) > PRECISION){
            err += 1;
            break;
        }
    }

    free(b);

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
#ifndef T6
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x){
        failed = failed + test6();
    }
    if (failed != 0){
        failcode = failcode + (1 << 5);
    }
#endif
    free(a);
    return failcode;
}