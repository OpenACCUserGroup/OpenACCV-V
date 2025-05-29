#include "acc_testsuite.h"
#ifndef T1
//T1:serial,loop,reduction,combined-constructs,V:2.6-2.7
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
        #pragma acc serial loop private(temp)
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
    return failcode;
}
