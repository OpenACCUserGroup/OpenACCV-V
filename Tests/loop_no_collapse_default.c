#include "acc_testsuite.h"
#ifndef T1
//T1:loop,construct-independent,V:1.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t * a = (real_t *)malloc(10 * n * sizeof(real_t));
    real_t * b = (real_t *)malloc(10 * n * sizeof(real_t));
    real_t * c = (real_t *)malloc(10 * n * sizeof(real_t));
    real_t total = 0;

    for (int x = 0; x < 10 * n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
    }
    for (int x = 0; x < 10; ++x){
        c[x * n] = a[x * n] + b[x * n];
    }

    #pragma acc data copyin(a[0:10*n], b[0:10*n]) copy(c[0:10*n])
    {
        #pragma acc parallel
        {
            #pragma acc loop independent
            for (int x = 0; x < 10; ++x){
                for (int y = 1; y < n; ++y){
                    c[x * n + y] = c[x * n + y - 1] + a[x * n + y] + b[x * n + y];
                }
            }
        }
    }
    for (int x = 0; x < 10; ++x){
        total = 0;
        for (int y = 0; y < n; ++y){
            total += a[x * n + y] + b[x * n +y];
            if (fabs(total - c[x * n + y]) > PRECISION * y){
                err += 1;
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
    return failcode;
}
