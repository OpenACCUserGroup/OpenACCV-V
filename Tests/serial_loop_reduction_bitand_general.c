#include "acc_testsuite.h"
#ifndef T1
//T1:serial,loop,reduction,combined-constructs,V:2.6-2.7
int test1(){
    int err = 0;
    srand(SEED);
    n = 10;
    unsigned int * a = (unsigned int *)malloc(n * sizeof(unsigned int));
    real_t false_margin = pow(exp(1), log(.5)/n);
    unsigned int temp = 1;
    unsigned int b = 0;
    unsigned int host_b;

    for (int x = 0; x < n; ++x){
        for (int y = 0; y < 16; ++y){
            if (rand() / (real_t) RAND_MAX < false_margin){
                for (int z = 0; z < y; ++z){
                    temp *= 2;
                }
                a[x] += temp;
                temp = 1;
            }
        }
    }
    for (int x = 0; x < 16; ++x){
        temp = 1;
        for (int y = 0; y < x; ++y){
            temp *= 2;
        }
        b += temp;
    }
    #pragma acc data copyin(a[0:n])
    {
        #pragma acc serial loop reduction(&:b)
        for (int x = 0; x < n; ++x){
            b = b & a[x];
        }
    }

    host_b = a[0];
    for (int x = 1; x < n; ++x){
        host_b = host_b & a[x];
    }
    if (b != host_b){
        err = 1;
    }

    return err;
}
#endif

int main(){
    int failcode = 0;
    int testrun;
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
