#include "acc_testsuite.h"
#ifndef T1
//T1:parallel,loop,reduction,combined-constructs,V:1.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    unsigned int * a = (unsigned int *)malloc(10 * n * sizeof(unsigned int));
    unsigned int * b = (unsigned int *)malloc(10 * n * sizeof(unsigned int));
    unsigned int * b_copy = (unsigned int *)malloc(10 * n * sizeof(unsigned int));
    unsigned int * c = (unsigned int *)malloc(10 * sizeof(unsigned int));
    unsigned int * host_c = (unsigned int *)malloc(10 * sizeof(unsigned int));

    real_t false_margin = pow(exp(1), log(.5)/n);
    unsigned int temp = 1;

    for (int x = 0; x < 10 * n; ++x){
        a[x] = 0;
        b[x] = (unsigned int) rand() / (real_t)(RAND_MAX / 1000);
        b_copy[x] = b[x];
        for (int y = 0; y < 16; ++y){
            if (rand() / (real_t) RAND_MAX < false_margin){
                a[x] += 1 << y;
            }
        }
    }
    #pragma acc data copyin(a[0:10 * n]) copy(b[0:10 * n], c[0:10])
    {
        #pragma acc parallel loop gang private(temp)
        for (int y = 0; y < 10; ++y){
            temp = a[y * n];
            #pragma acc loop worker reduction(&:temp)
            for (int x = 1; x < n; ++x){
                temp = temp & a[y * n + x];
            }
            c[y] = temp;
            #pragma acc loop worker
            for (int x = 0; x < n; ++x){
                b[y * n + x] = b[y * n + x] + c[y];
            }
        }
    }
    for (int x = 0; x < 10; ++x){
        host_c[x] = a[x * n];
        for (int y = 1; y < n; ++y){
            host_c[x] = host_c[x] & a[x * n + y];
        }
        if (host_c[x] != c[x]){
          err += 1;
        }
    }

    for (int x = 0; x < 10; ++x){
        for (int y = 0; y < n; ++y){
            if (b[x * n + y] != b_copy[x * n + y] + c[x]){
                err += 1;
            }
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
    unsigned int * b = (unsigned int *)malloc(25 * n * sizeof(unsigned int));
    unsigned int * b_copy = (unsigned int *)malloc(25 * n * sizeof(unsigned int));
    unsigned int * c = (unsigned int *)malloc(25 * sizeof(unsigned int));
    unsigned int device[5];
    unsigned int host[5];

    real_t false_margin = pow(exp(1), log(.5)/n);
    
    for (int x = 0; x < 25 * n; ++x){
        a[x] = 0;
        b[x] = (unsigned int) rand() / (real_t)(RAND_MAX / 1000);
        b_copy[x] = b[x];
        for (int y = 0; y < 16; ++y) {
            if (rand() / (real_t)RAND_MAX < false_margin) {
                a[x] += 1 << y;
            }
        }
    }

    #pragma acc data copyin(a[0:25*n]) copy(b[0:25*n], c[0:25])
    {
        #pragma acc parallel loop gang private(device)
        for (int x = 0; x < 5; ++x) {
            for (int y = 0; y < 5; ++y) {
                device[y] = a[x * 5 * n + y];
            }
            #pragma acc loop worker reduction(&:device)
            for (int y = 0; y < 5 * n; ++y) {
                device[y%5] = device[y%5] & a[x * 5 * n + y];
            }
            for (int y = 0; y < 5; ++y) {
                c[x * 5 + y] = device[y];
            }
            #pragma acc loop worker
            for (int y = 0; y < 5 * n; ++y) {
                b[x * 5 * n + y] = b[x * 5 * n + y] + c[x * 5 + (y % 5)];
            }
        }
    }

    for (int x = 0; x < 5; ++x) {
        for (int y = 0; y < 5; ++y) {
            host[y] = a[x * 5 * n + y];
        }
        for (int y = 0; y < 5 * n; ++y) {
            host[y%5] = host[y%5] & a[x * 5 * n + y];
        }
        for (int y = 0; y < 5; ++y) {
            if (host[y] != c[x * 5 + y]) {
                err += 1;
            }
        }
        for (int y = 0; y < 5 * n; ++y) {
            if (b[x * 5 * n + y] != (host[y%5] + b_copy[x * 5 * n + y])) {
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
