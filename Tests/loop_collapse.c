#include "acc_testsuite.h"
#ifndef T1
//T1:loop,construct-independent,V:1.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t * a = (real_t *)malloc(10 * n * sizeof(real_t));
    real_t * b = (real_t *)malloc(10 * n * sizeof(real_t));
    real_t * c = (real_t *)malloc(10 * n * sizeof(real_t));

    for (int x = 0; x < 10 * n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0;
    }
    for (int x = 0; x < 10; ++x){
        c[x * n] = a[x * n] + b[x * n];
    }

    #pragma acc data copyin(a[0:10*n], b[0:10*n]) copy(c[0:10*n])
    {
        #pragma acc parallel
        {
            #pragma acc loop independent collapse(1)
            for (int x = 0; x < 10; ++x){
                for (int y = 1; y < n; ++y){
                    c[x * n + y] = c[x * n + y - 1] + a[x * n + y] + b[x * n + y];
                }
            }
        }
    }

    real_t total;
    for (int x = 0; x < 10; ++x){
        total = 0;
        for (int y = 0; y < n; ++y){
            total += a[x * n + y] + b[x * n + y];
            if (fabs(c[x * n + y] - total) > PRECISION){
                err += 1;
            }
        }
    }

    return err;
}
#endif

#ifndef T2
//T2:loop,syntactic,construct-independent,V:1.0-2.7
int test2(){
    int err = 0;
    srand(SEED);
    real_t * a = (real_t *)malloc(10 * n * sizeof(real_t));
    real_t * b = (real_t *)malloc(10 * n * sizeof(real_t));
    real_t * c = (real_t *)malloc(10 * n * sizeof(real_t));

    for (int x = 0; x < 10 * n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0.0;
    }

    #pragma acc data copyin(a[0:10*n], b[0:10*n]) copyout(c[0:10*n])
    {
        #pragma acc parallel
        {
            #pragma acc loop independent collapse(2)
            for (int x = 0; x < 10; ++x){
                for (int y = 0; y < n; ++y){
                    c[x * n + y] = a[x * n + y] + b[x * n + y];
                }
            }
        }
    }

    for (int x = 0; x < 10 * n; ++x){
        if (fabs(c[x] - (a[x] + b[x])) > PRECISION){
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
    return failcode;
}
