#include "acc_testsuite.h"
#ifndef T1
//T1:parallel,loop,reduction,combined-constructs,V:1.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    unsigned int * a = (unsigned int *)malloc(n * sizeof(unsigned int));
    real_t false_margin = pow(exp(1), log(.5)/n);
    unsigned int temp = 1;
    unsigned int b = 0;
    unsigned int host_b;

    for (int x = 0; x < n; ++x){
        a[x] = 0;
        for (int y = 0; y < 16; ++y){
            if (rand() / (real_t) RAND_MAX > false_margin){
                a[x] += 1 << y;
            }
        }
    }

    #pragma acc data copyin(a[0:n])
    {
        #pragma acc parallel loop reduction(|:b)
        for (int x = 0; x < n; ++x){
            b = b | a[x];
        }
    }

    host_b = a[0];
    for (int x = 1; x < n; ++x){
        host_b = host_b | a[x];
    }

    if (b != host_b){
        err = 1;
    }

    return err;
}
#endif

#ifndef T2
//T2:parallel,reduction,combined-constructs,loop,V:2.7-2.7
int test2(){
    int err = 0;
    srand(SEED);
    unsigned int * a = (unsigned int *)malloc(5 * n * sizeof(unsigned int));
    real_t false_margin = pow(exp(1), log(.5)/n);
    unsigned int b[5];
    unsigned int host_b[5];

    for (int x = 0; x < 5 * n; ++x){
        a[x] = 0;
        for (int y = 0; y < 16; ++y) {
            if (rand() / (real_t) RAND_MAX > false_margin) {
                a[x] += 1 << y;
            }
        }
    }

    for (int x = 0; x < 5; ++x) {
        b[x] = 0;
        host_b[x] = 0;
    }

    #pragma acc data copyin(a[0:5*n])
    {
        #pragma acc parallel loop reduction(|:b)
        for (int x = 0; x < 5 * n; ++x) {
            b[x%5] = b[x%5] | a[x];
        }
    }

    for (int x = 0; x < 5 * n; ++x){
        host_b[x%5] = host_b[x%5] | a[x];
    }

    for (int x = 0; x < 5; ++x) {
        if (host_b[x] != b[x]) {
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
