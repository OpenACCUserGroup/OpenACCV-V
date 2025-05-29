#include "acc_testsuite.h"
#ifndef T1
//T1:parallel,reduction,combined-constructs,loop,V:1.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    unsigned int * a = (unsigned int *)malloc(n * sizeof(unsigned int));
    unsigned int b = 0;
    unsigned int host_b = 0;

    for (int x = 0; x < n; ++x){
        a[x] = (unsigned int) rand() / (real_t) (RAND_MAX / 2);
    }

    #pragma acc data copyin(a[0:n])
    {
        #pragma acc parallel loop reduction(^:b)
        for (int x = 0; x < n; ++x){
            b = b ^ a[x];
        }
    }

    for (int x = 0; x < n; ++x){
        host_b = host_b ^ a[x];
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
    unsigned int * a = (unsigned int *)malloc(10 * n * sizeof(unsigned int));
    unsigned int b[10];
    unsigned int host_b[10];

    for (int x = 0; x < 10 * n; ++x){
        a[x] = (unsigned int) rand() / (real_t) (RAND_MAX / 2);
    }

    for (int x = 0; x < 10; ++x) {
        b[x] = 0;
        host_b[x] = 0;
    }

    #pragma acc data copyin(a[0:10*n])
    {
        #pragma acc parallel loop reduction(^:b)
        for (int x = 0; x < 10 * n; ++x) {
            b[x % 10] = b[x % 10] ^ a[x];
        }
    }

    for (int x = 0; x < 10 * n; ++x) {
        host_b[x % 10] = host_b[x % 10] ^ a[x];
    }

    for (int x = 0; x < 10; ++x) {
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
