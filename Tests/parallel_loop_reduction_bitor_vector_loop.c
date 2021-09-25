#include "acc_testsuite.h"
#ifndef T1
//T1:parallel,loop,reduction,combined-constructs,V:1.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    unsigned int* a = (unsigned int *)malloc(10 * n * sizeof(unsigned int));
    unsigned int* b = (unsigned int *)malloc(10 * sizeof(unsigned int));

    real_t false_margin = pow(exp(1), log(.5)/n);
    unsigned int temp = 1;
    unsigned int b_host;

    for (int x = 0; x < 10 * n; ++x){
        for (int y = 0; y < 16; ++y){
            if (rand() / (real_t) RAND_MAX > false_margin){
                for (int z = 0; z < y; ++z){
                    temp *= 2;
                }
                a[x] += temp;
                temp = 1;
            }
        }
    }
    temp = 0;
    #pragma acc data copyin(a[0:10*n]) copy(b[0:10])
    {
        #pragma acc parallel loop private(temp)
        for (int x = 0; x < 10; ++x){
            temp = 0;
            #pragma acc loop vector reduction(|:temp)
            for (int y = 0; y < n; ++y){
                temp = temp | a[x * n + y];
            }
            b[x] = temp;
        }
    }

    for (int x = 0; x < 10; ++x){
        b_host = a[x * n];
        for (int y = 1; y < n; ++y){
            b_host = b_host | a[x * n + y];
        }
        if (b_host != b[x]){
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
    unsigned int * a = (unsigned int *)malloc(25 * n * sizeof(unsigned int));
    unsigned int * b = (unsigned int *)malloc(25 * sizeof(unsigned int));

    unsigned int reduced[5];
    real_t false_margin = pow(exp(1), log(.5)/n);
    
    for (int x = 0; x < 25 * n; ++x) {
        a[x] = 0;
        for (int y = 0; y < 16; ++y) {
            if (rand() / (real_t)RAND_MAX > false_margin) {
                a[x] += 1<<y;
            }
        }
    }

    #pragma acc data copyin(a[0:25*n]) copy(b[0:25])
    {
        #pragma acc parallel loop private(reduced)
        for (int x = 0; x < 5; ++x) {
            for (int y = 0; y < 5; ++y) {
                reduced[y] = 0;
            }
            #pragma acc loop vector reduction(|:reduced)
            for (int y = 0; y < 5 * n; ++y) {
                reduced[y%5] = reduced[y%5] | a[x * 5 * n + y];
            }
            for (int y = 0; y < 5; ++y) {
                b[x * 5 + y] = reduced[y];
            }
        }
    }

    for (int x = 0; x < 5; ++x) {
        for (int y = 0; y < 5; ++y) {
            reduced[y] = 0;
        }
        for (int y = 0; y < 5 * n; ++y) {
            reduced[y%5] = reduced[y%5] | a[x * 5 * n + y];
        }
        for (int y = 0; y < 5; ++y) {
            if (b[x * 5 + y] != reduced[y]) {
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
