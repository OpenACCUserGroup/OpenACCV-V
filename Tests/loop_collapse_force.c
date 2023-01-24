#include "acc_testsuite.h"

#ifndef T1
//T1:loop,construct-independent,combined-constructs,V:3.3
int test1(){
    int err = 0;
    srand(SEED);
    real_t * a = (real_t *)malloc(10 * n * sizeof(real_t));
    real_t * b = (real_t *)malloc(10 * n * sizeof(real_t));
    real_t * c = (real_t *)malloc(10 * n * sizeof(real_t));

    real_t * i2 = (real_t *)malloc(10 * sizeof(real_t));
    real_t * i3 = (real_t *)malloc(10 * sizeof(real_t));

    real_t nx;
    real_t ny;

    for (int x = 0; x < 10 * n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0.0;
        i2[x] = rand() / (real_t)(RAND_MAX / 10);
        i3[x] = rand() / (real_t)(RAND_MAX / 10);
    }

    #pragma acc data copyin(a[0:10*n], b[0:10*n]) copyout(c[0:10*n])
    {
        #pragma acc parallel loop collapse(2)
        for (int x = 0; x < 10; ++x){
            nx = i2[x];
            ny = i3[x];
            for (int y = 0; y < n; ++y){
                c[x * n + y] = nx * a[x * n + y] + ny * b[x * n + y];
            }
        }
    }

    for (int x = 0; x < 10 * n; ++x){
        if (fabs(c[x] - (i2[x] * a[x] + i3[x] * b[x])) > PRECISION){
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
