#include "acc_testsuite.h"
#ifndef T1
//T1:serial,loop,reduction,combined-constructs,V:2.6-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t * a = (real_t *)malloc(10 * n * sizeof(real_t));
    real_t * b = (real_t *)malloc(10 * n * sizeof(real_t));
    real_t * c = (real_t *)malloc(10 * sizeof(real_t));
    real_t temp = 0.0;

    for(int x = 0; x < 10 * n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
    }
    #pragma acc data copyin(a[0:10*n], b[0:10*n]) copyout(c[0:10])
    {
        #pragma acc serial loop private(temp)
        for (int x = 0; x < 10; ++x){
            temp = 0.0;
            #pragma acc loop vector reduction(+:temp)
            for (int y = 0; y < n; ++y){
                temp += a[(x * n) + y] + b[(x * n) + y];
            }
            c[x] = temp;
        }
    }
    for (int x = 0; x < 10; ++x){
        for (int y = 0; y < n; ++y){
            c[x] -= a[(x * n) + y] + b[(x * n) + y];
        }
        if (fabs(c[x]) > PRECISION * (2 * n - 1)){
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
    return failcode;
}
