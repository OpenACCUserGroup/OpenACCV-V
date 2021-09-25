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
    unsigned int c = 0;
    unsigned int b_host;

    for (int x = 0; x < 10 * n; ++x){
        a[x] = 0;
        for (int y = 0; y < 16; ++y){
            if (rand() / (real_t) RAND_MAX < false_margin){
                a[x] += 1 << y;
            }
        }
    }

    #pragma acc data copyin(a[0:10*n]) copyout(b[0:10])
    {
        #pragma acc parallel loop private(c)
        for (int x = 0; x < 10; ++x){
            c = a[x * n];
            #pragma acc loop vector reduction(&:c)
            for (int y = 1; y < n; ++y){
                c = c & a[x * n + y];
            }
            b[x] = c;
        }
    }
    for (int x = 0; x < 10; ++x){
        b_host = a[x * n];
        for (int y = 1; y < n; ++y){
            b_host = b_host & a[x * n + y];
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
    real_t false_margin = pow(exp(1), log(.5)/n);
    unsigned int c[5];
    unsigned int * b_host = (unsigned int *)malloc(5 * sizeof(unsigned int));
    

    for (int x = 0; x < 25 * n; ++x){
        a[x] = 0;
        for (int y = 0; y < 16; ++y) {
            if (rand() / (real_t)RAND_MAX < false_margin) {
                a[x] += 1 << y;
            }
        }
    }

    #pragma acc data copyin(a[0:25*n]) copyout(b[0:25])
    {
        #pragma acc parallel loop private(c)
        for (int x = 0; x < 5; ++x) {
            for (int y = 0; y < 5; ++y) {
                c[y] = a[x * 5 * n + y];
            }
            #pragma acc loop vector reduction(&:c)
            for (int y = 0; y < 5 * n; ++y) {
                c[y%5] = c[y%5] & a[x * 5 * n + y];
            }
            for (int y = 0; y < 5; ++y) {
                b[x * 5 + y] = c[y];
            }
        }
    }

    for (int x = 0; x < 5; ++x) {
        for (int y = 0; y < 5; ++y) {
            b_host[y] = a[x * 5 * n + y];
        }
        for (int y = 0; y < 5 * n; ++y) {
            b_host[y%5] = b_host[y%5] & a[x * 5 * n + y];
        }
        for (int y = 0; y < 5; ++y) {
            if (b_host[y] != b[x * 5 + y]){
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
