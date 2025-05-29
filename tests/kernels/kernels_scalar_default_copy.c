#include "acc_testsuite.h"
#ifndef T1
//T1:kernels,data,data-region,default-mapping,V:2.5-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t * a = (real_t *)malloc(n * sizeof(real_t));
    real_t * b = (real_t *)malloc(n * sizeof(real_t));
    real_t sum = 0.0;

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
    }

    for (int x = 0; x < n; ++x){
        sum = sum + a[x];
    }

    #pragma acc data copyin(b[0:n])
    {
        #pragma acc kernels num_gangs(1) vector_length(1)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                sum = sum + b[x];
            }
        }
    }

    for (int x = 0; x < n; ++x){
        sum = sum - (a[x] + b[x]);
    }

    if (fabs(sum) > (2 * n - 2) * PRECISION){
        err += 1;
        sum = 0.0;
        for (int x = 0; x < n; ++x){
            sum = sum + a[x];
        }
        sum = 0.0;
        for (int x = 0; x < n; ++x){
            sum = sum + b[x];
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
