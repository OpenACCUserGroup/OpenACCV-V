#include "acc_testsuite.h"
#ifndef T1
//T1:parallel,loop,reduction,combined-constructs,V:1.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t * a = (real_t *)malloc(10 * n * sizeof(real_t));
    real_t * b = (real_t *)malloc(10 * n * sizeof(real_t));
    real_t * min = (real_t *)malloc(10 * sizeof(real_t));
    real_t temp = 100;
    real_t temp_min;

    for (int x = 0; x < 10 * n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX/10);
        b[x] = rand() / (real_t)(RAND_MAX/10);
    }

    #pragma acc data copyin(a[0:10*n], b[0:10*n]) copy(min[0:10])
    {
        #pragma acc parallel loop gang private(temp)
        for (int x = 0; x < 10; ++x){
            temp = 100;
            #pragma acc loop vector reduction(min:temp)
            for (int y = 0; y < n; ++y){
                temp = fmin(a[x * n + y] * b[x * n + y], temp);
            }
            min[x] = temp;
        }
    }
    for (int x = 0; x < 10; ++x){
        temp_min = 100;
        for (int y = 0; y < n; ++y){
            if (temp_min > a[x * n + y] * b[x * n + y]){
                temp_min = a[x * n + y] * b[x * n + y];
            }
        }
        if (fabs(temp_min - min[x]) > PRECISION){
            err = 1;
        }
    }

    return err;
}
#endif

#ifndef T2
//T2:parallel,private,reduction,combined-constructs,loop,V:2.7-2.7
int test2(){
    int err = 0;
    srand(SEED);
    real_t * a = (real_t *)malloc(25 * n * sizeof(real_t));
    real_t * b = (real_t *)malloc(25 * n * sizeof(real_t));
    real_t * minimums = (real_t *)malloc(25 * sizeof(real_t));
    real_t reduced[5];

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
    }

    #pragma acc data copyin(a[0:25*n], b[0:25*n]) copy(minimums[0:25])
    {
        #pragma acc parallel loop gang private(reduced)
        for (int x = 0; x < 5; ++x) {
            for (int y = 0; y < 5; ++y) {
                reduced[y] = 1000;
            }
            #pragma acc loop vector reduction(min:reduced)
            for (int y = 0; y < 5 * n; ++y) {
                reduced[y%5] = fmin(a[x * 5 * n + y] * b[x * 5 * n + y], reduced[y % 5]);
            }
            for (int y = 0; y < 5; ++y) {
                minimums[x * 5 + y] = reduced[y];
            }
        }
    }

    for (int x = 0; x < 5; ++x) {
        for (int y = 0; y < 5; ++y) {
            reduced[y] = 1000;
        }
        for (int y = 0; y < 5 * n; ++y) {
            reduced[y%5] = fmin(a[x * 5 * n + y] * b[x * 5 * n + y], reduced[y % 5]);
        }
        for (int y = 0; y < 5; ++y) {
            if (fabs(reduced[y] - minimums[x * 5 + y]) > PRECISION) {
                err += 1;
            }
        }
    }

    return err;
}
#endif

int main(){
    int failcode = 0;
    int testrun;
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
