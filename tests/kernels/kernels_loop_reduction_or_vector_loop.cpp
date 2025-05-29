#include "acc_testsuite.h"
#ifndef T1
//T1:kernels,loop,reduction,combined-constructs,V:1.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    char * a = new char[10 * n];
    char * b = new char[10];
    real_t false_margin = pow(exp(1), log(.5)/n);
    char temp = 0;
    char found;

    for (int x = 0; x < 10 * n; ++x){
        if(rand() / (real_t)(RAND_MAX) > false_margin){
            a[x] = 1;
        }
        else{
            a[x] = 0;
        }
    }
    #pragma acc data copyin(a[0:10*n]) copy(b[0:10])
    {
        #pragma acc kernels loop private(temp)
        for (int x = 0; x < 10; ++x){
            temp = 0;
            #pragma acc loop vector reduction(||:temp)
            for (int y = 0; y < n; ++y){
                temp = temp || a[x * n + y];
            }
            b[x] = temp;
        }
    }
    for (int x = 0; x < 10; ++x){
        found = 0;
        for (int y = 0; y < n; ++y){
            if (a[x * n + y] &! 0){
                found = 1;
            }
        }
        if (found != b[x]){
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
