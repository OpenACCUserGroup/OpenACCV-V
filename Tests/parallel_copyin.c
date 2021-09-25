#include "acc_testsuite.h"
#ifndef T1
//T1:parallel,data,data-region,V:2.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t * a = (real_t *)malloc(n * sizeof(real_t));
    real_t * a_copy = (real_t *)malloc(n * sizeof(real_t));
    real_t * b = (real_t *)malloc(n * sizeof(real_t));
    int* hasDevice = (int *) malloc(sizeof(int));
    hasDevice[0] = 1;
    #pragma acc enter data copyin(hasDevice[0:1])
    #pragma acc parallel present(hasDevice[0:1])
    {
      hasDevice[0] = 0;
    }

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        a_copy[x] = a[x];
        b[x] = 0.0;
    }

    #pragma acc parallel copyin(a[0:n])
    {
        #pragma acc loop
        for (int x = 0; x < n; ++x){
            a[x] = 0.0;
        }
    }

    for (int x = 0; x < n; ++x){
        if (((fabs(a[x] - a_copy[x]) > PRECISION) && (hasDevice[0] == 1)) || ((hasDevice[0] == 0) && (fabs(a[x]) > PRECISION))){
            err = 1;
        }
    }

    return err;
}
#endif

#ifndef T2
//T2:parallel,data,data-region,V:1.0-2.7
int test2(){
    int err = 0;
    srand(SEED);
    real_t * a = (real_t *)malloc(n * sizeof(real_t));
    real_t * a_copy = (real_t *)malloc(n * sizeof(real_t));
    real_t * b = (real_t *)malloc(n * sizeof(real_t));
    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
    }

    #pragma acc data copy(b[0:n])
    {
        #pragma acc parallel copyin(a[0:n])
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                b[x] = a[x];
            }
        }
    }

    for (int x = 0; x < n; ++x){
        if (fabs(a[x] - b[x]) > PRECISION){
            err = 1;
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
