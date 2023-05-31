#include "acc_testsuite.h"
#ifndef T1
//T1:parallel,loop,reduction,combined-constructs,V:1.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    unsigned int* a = (unsigned int *)malloc(10 * n * sizeof(unsigned int));
    unsigned int* b = (unsigned int *)malloc(10 * sizeof(unsigned int));

    unsigned int temp = 0;
    unsigned int b_host;

    for (int x = 0; x < 10 * n; ++x){
        a[x] = (unsigned int) rand() / (real_t) (RAND_MAX / 2);
    }

    #pragma acc data copyin(a[0:10*n]) copy(b[0:10])
    {
        #pragma acc parallel loop private(temp)
        for (int x = 0; x < 10; ++x){
            temp = 0;
            #pragma acc loop vector reduction(^:temp)
            for (int y = 0; y < n; ++y){
                temp = temp ^ a[x * n + y];
            }
            b[x] = temp;
        }
    }

    for (int x = 0; x < 10; ++x){
        b_host = a[x * n];
        for (int y = 1; y < n; ++y){
            b_host = b_host ^ a[x * n + y];
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

    unsigned int temp[5];

    for (int x = 0; x < 25 * n; ++x){
        a[x] = (unsigned int) rand() / (real_t) (RAND_MAX / 2);
    }

    #pragma acc data copyin(a[0:25*n]) copy(b[0:25])
    {
        #pragma acc parallel loop private(temp)
        for (int x = 0; x < 5; ++x) {
            for (int y = 0; y < 5; ++y){
                temp[y] = 0;
            }
            #pragma acc loop vector reduction(^:temp)
            for (int y = 0; y < 5 * n; ++y) {
                temp[y % 5] = temp[y % 5] ^ a[x * 5 * n + y];
            }
            for (int y = 0; y < 5; ++y) {
                b[x * 5 + y] = temp[y];
            }
        }
    }

    for (int x = 0; x < 5; ++x) {
        for (int y = 0; y < 5; ++y) {
            temp[y] = 0;
        }
        for (int y = 0; y < 5 * n; ++y) {
            temp[y % 5] = temp[y % 5] ^ a[x * 5 * n + y];
        }
        for (int y = 0; y < 5; ++y) {
            if (b[x * 5 + y] != temp[y]) {
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
