#include "acc_testsuite.h"
#ifndef T1
//T1:parallel,private,reduction,combined-constructs,loop,V:1.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t * a = new real_t[10 * n];
    real_t * b = new real_t[10 * n];
    real_t * c = new real_t[10];
    real_t temp = 0.0;

    for(int x = 0; x < 10 * n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
    }
    #pragma acc data copyin(a[0:10*n], b[0:10*n]) copyout(c[0:10])
    {
        #pragma acc parallel loop private(temp)
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

#ifndef T2
//T2:parallel,private,reduction,combined-constructs,loop,V:2.7-2.7
int test2(){
    int err = 0;
    srand(SEED);
    real_t * a = new real_t[25 * n];
    real_t * b = new real_t[25 * n];
    real_t * c = new real_t[25];
    real_t * c_host = new real_t[25];
    real_t temp[5];

    for (int x = 0; x < 25 * n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
    }

    for (int x = 0; x < 25; ++x) {
        c[x] = 0;
        c_host[x] = 0;
    }

    #pragma acc data copyin(a[0:25*n], b[0:25*n]) copyout(c[0:25])
    {
        #pragma acc parallel loop private(temp)
        for (int x = 0; x < 5; ++x) {
            for (int y = 0; y < 5; ++y) {
                temp[y] = 0;
            }
            #pragma acc loop vector reduction(+:temp)
            for (int y = 0; y < 5 * n; ++y) {
                temp[y % 5] += a[x * 5 * n + y] + b[x * 5 * n + y];
            }
            for (int y = 0; y < 5; ++y) {
                c[x * 5 + y] = temp[y];
            }
        }
    }

    for (int x = 0; x < 5; ++x) {
        for (int y = 0; y < 5 * n; ++y) {
            c_host[x * 5 + (y % 5)] += a[x * 5 * n + y] + b[x * 5 * n + y];
        }
    }

    for (int x = 0; x < 25; ++x) {
        if (fabs(c[x] - c_host[x]) > PRECISION) {
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
