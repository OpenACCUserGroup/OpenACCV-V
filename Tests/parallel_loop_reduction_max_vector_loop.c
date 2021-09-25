#include "acc_testsuite.h"
#ifndef T1
//T1:parallel,loop,reduction,combined-constructs,V:1.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t * a = (real_t *)malloc(10 * n * sizeof(real_t));
    real_t * b = (real_t *)malloc(10 * n * sizeof(real_t));
    real_t * max = (real_t *)malloc(10 * sizeof(real_t));
    real_t temp = 0;
    real_t temp_max;

    for (int x = 0; x < 10 * n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX/10);
        b[x] = rand() / (real_t)(RAND_MAX/10);
    }
    #pragma acc data copyin(a[0:10*n], b[0:10*n]) copy(max[0:10])
    {
        #pragma acc parallel loop private(temp)
        for (int x = 0; x < 10; ++x){
            temp = 0;
            #pragma acc loop vector reduction(max:temp)
            for (int y = 0; y < n; ++y){
                temp = fmax(a[x * n + y] * b[x * n + y], temp);
            }
            max[x] = temp;
        }
    }
    for (int x = 0; x < 10; ++x){
        temp_max = 0;
        for (int y = 0; y < n; ++y){
            if (temp_max < a[x * n + y] * b[x * n + y]){
                temp_max = a[x * n + y] * b[x * n + y];
            }
        }
        if (fabs(temp_max - max[x]) > PRECISION){
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
    real_t * maximums = (real_t *)malloc(25 * sizeof(real_t));
    real_t reduced[5];

    for (int x = 0; x < 25 * n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
    }

    for (int x = 0; x < 25; ++x) {
        maximums[x] = 0;
    }

    #pragma acc data copyin(a[0:25*n], b[0:25*n]) copy(maximums[0:25])
    {
        #pragma acc parallel loop private(reduced)
        for (int x = 0; x < 5; ++x) {
            for (int y = 0; y < 5; ++y) {
                reduced[y] = 0;
            }
            #pragma acc loop vector reduction(max:reduced)
            for (int y = 0; y < 5 * n; ++y) {
                reduced[y%5] = fmax(a[x * 5 * n + y] * b[x * 5 * n + y], reduced[y%5]);
            }
            for (int y = 0; y < 5; ++y) {
                maximums[x * 5 + y] = reduced[y];
            }
        }
    }

    for (int x = 0; x < 5; ++x) {
        for (int y = 0; y < 5; ++y) {
            reduced[y] = 0;
        }
        for (int y = 0; y < 5 * n; ++y) {
            reduced[y%5] = fmax(a[x * 5 * n + y] * b[x * 5 * n + y], reduced[y%5]);
        }
        for (int y = 0; y < 5; ++y) {
            if (fabs(maximums[x * 5 + y] - reduced[y]) > PRECISION) {
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
