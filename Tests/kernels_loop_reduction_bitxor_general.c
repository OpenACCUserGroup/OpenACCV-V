#include "acc_testsuite.h"
#ifndef T1
//T1:kernels,loop,reduction,combined-constructs,V:1.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    unsigned int * a = (unsigned int *)malloc(n * sizeof(unsigned int));
    unsigned int b = 0;

    for (int x = 0; x < n; ++x){
        a[x] = (unsigned int) rand() / (real_t) (RAND_MAX / 2);
    }
    unsigned int host_b = a[0];

    #pragma acc data copyin(a[0:n])
    {
        #pragma acc kernels loop reduction(^:b)
        for (int x = 0; x < n; ++x){
            b = b ^ a[x];
        }
    }


    for (int x = 1; x < n; ++x){
        host_b = host_b ^ a[x];
    }
    if (b != host_b){
        err = 1;
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
