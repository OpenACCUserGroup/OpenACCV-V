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

    real_t false_margin = pow(exp(1), log(.5)/n);
    unsigned int temp = 1;
    unsigned int host_c;

    for (int x = 0; x < 10; ++x){
        for (int y = 0; y < n; ++y){
            b[x * n + y] = (unsigned int) rand() / (real_t)(RAND_MAX / 1000);
            b_copy[x * n + y] = b[x * n + y];
            for (int z = 0; z < 16; ++z){
                a[x * n + y] += (1<<z);
            }
        }
    }

    #pragma acc data copyin(a[0:10*n]) copy(b[0:10*n], c[0:10])
    {
        #pragma acc parallel loop gang private(temp)
        for (int x = 0; x < 10; ++x){
            temp = 0;
            #pragma acc loop worker reduction(|:temp)
            for (int y = 0; y < n; ++y){
                temp = temp | a[x * n + y];
            }
            c[x] = temp;
            #pragma acc loop worker
            for (int y = 0; y < n; ++y){
                b[x * n + y] = b[x * n + y] + c[x];
            }
        }
    }
    for (int x = 0; x < 10; ++x){
        host_c = a[x * n];
        for (int y = 1; y < n; ++y){
            host_c = host_c | a[x * n + y];
        }
        if (host_c != c[x]){
            err += 1;
        }
        for (int y = 0; y < n; ++y){
            if (b[x * n + y] != b_copy[x * n + y] + host_c){
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
    unsigned int temp[5];

    real_t false_margin = pow(exp(1), log(.5)/n);

    for (int x = 0; x < 25 * n; ++x){
        b[x] = (unsigned int) rand() / (real_t)(RAND_MAX / 1000);
        b_copy[x] = b[x];
        for (int y = 0; y < 16; ++y) {
            if (rand() / (real_t)RAND_MAX > false_margin) {
                a[x] += (1<<y);
            }
        }
    }

    #pragma acc data copyin(a[0:25*n]) copy(b[0:25*n], c[0:25])
    {
        #pragma acc parallel loop gang private(temp)
        for (int x = 0; x < 5; ++x) {
            for (int y = 0; y < 5; ++y) {
                temp[y] = 0;
            }
            #pragma acc loop worker reduction(|:temp)
            for (int y = 0; y < 5 * n; ++y) {
                temp[y%5] = temp[y%5] | a[x * 5 * n + y];
            }
            for (int y = 0; y < 5; ++y) {
                c[x * 5 + y] = temp[y];
            }
            #pragma acc loop worker
            for (int y = 0; y < 5 * n; ++y) {
                b[x * 5 * n + y] = b[x * 5 * n + y] + c[x * 5 + (y % 5)];
            }
        }
    }

    for (int x = 0; x < 5; ++x){
        for (int y = 0; y < 5; ++y){
            temp[y] = 0;
        }
        for (int y = 0; y < 5 * n; ++y) {
            temp[y%5] = temp[y%5] | a[x * 5 * n + y];
        }
        for (int y = 0; y < 5; ++y){
            if (c[x * 5 + y] != temp[y]) {
                err += 1;
            }
        }
        for (int y = 0; y < 5 * n; ++y) {
            if ((b_copy[x * 5 * n + y] + c[x * 5 + (y % 5)]) != b[x * 5 * n + y]) {
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
