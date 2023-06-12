#include "acc_testsuite.h"
#ifndef T1
//T1:serial,data,data-region,V:3.0-3.1
int test1(){
    int err = 0;
    srand(SEED);

    real_t * a = new real_t[n];
    real_t * b = new real_t[n];

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = 0.0;
    }

    #pragma acc data copyin(a[0:n])
    {
        #pragma acc serial copyout(zero: b[0:n])
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                b[x] += a[x];
            }
        }
    }

    for (int x = 0; x < n; ++x){
        if (fabs(a[x] - b[x]) > PRECISION){
            err += 1;
            break;
        }
    }

    return err;
}
#endif

#ifndef T2
//T2:serial,data,data-region,reference-counting,devonly,V:3.0-3.1
int test2(){
    int err = 0;
    srand(SEED);

    real_t * a = new real_t[n];
    real_t * b = new real_t[n];
    int* hasDevice = (int *) malloc(sizeof(int));
    hasDevice[0] = 1;
    #pragma acc enter data copyin(hasDevice[0:1])
    #pragma acc serial present(hasDevice[0:1])
    {
      hasDevice[0] = 0;
    }

    if (hasDevice[0] == 1){
        for (int x = 0; x < n; ++x){
            a[x] = rand() / (real_t)(RAND_MAX / 10);
            b[x] = 0;
        }
        #pragma acc data copyin(a[0:n], b[0:n])
        {
            #pragma acc serial copyout(b[0:n])
            {
                #pragma acc loop
                for (int x = 0; x < n; ++x){
                    b[x] += a[x];
                }
            }
        }
        for (int x = 0; x < n; ++x){
            if (fabs(b[x]) > PRECISION){
                err += 1;
                break;
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
