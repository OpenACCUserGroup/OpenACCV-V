#include "acc_testsuite.h"
#ifndef T1
//T1:data,default-mapping,serial,firstprivate,V:2.6-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t * a = (real_t *)malloc(n * sizeof(real_t));
    real_t * b = (real_t *)malloc(n * sizeof(real_t));
    real_t scalar = rand() / (real_t)(RAND_MAX / 10);
    real_t scalar_copy = scalar;

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = 0.0;
    }

    #pragma acc data copyin(a[0:n]) copyout(b[0:n])
    {
        #pragma acc serial
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                b[x] = a[x] + scalar;
            }
        }
    }

    for (int x = 0; x < n; ++x){
        if (fabs(b[x] - (a[x] + scalar_copy)) > PRECISION){
            err += 1;
        }
    }
    if (fabs(scalar_copy - scalar) > PRECISION){
        err += 1;
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
    return failcode;
}
