#include "acc_testsuite.h"
#pragma acc routine worker nohost
real_t function(real_t * a, long long n){
    real_t returned = 0;
    #pragma acc loop worker reduction(+:returned)
    for (int x = 0; x < n; ++x){
        returned += a[x];
    }
    return returned;
}

#ifndef T1
//T1:routine,construct-independent,V:2.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t ** a = (real_t **)malloc(n * sizeof(real_t *));
    real_t * b = new real_t[n];
    real_t summation;

    for (int x = 0; x < n; ++x){
        a[x] = new real_t[n];
        for (int y = 0; y < n; ++y){
            a[x][y] = rand() / (real_t)(RAND_MAX / 10);
        }
        b[x] = 0;
    }

    #pragma acc data copyin(a[0:n][0:n]) copyout(b[0:n])
    {
        #pragma acc parallel
        {
            #pragma acc loop gang
            for (int x = 0; x < n; ++x){
                b[x] = function(a[x], n);
            }
        }
    }

    for (int x = 0; x < n; ++x){
        summation = 0;
        for (int y = 0; y < n; ++y){
            summation += a[x][y];
        }
        if (fabs(summation - b[x]) > PRECISION){
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
    return failcode;
}
