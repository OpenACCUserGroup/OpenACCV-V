#include "acc_testsuite.h"
#ifndef T1
//T1:kernels,loop,reduction,combined-constructs,V:1.0-2.7
int test1(){
    int err = 0;
    srand(SEED);

    char * a = new char[n];
    real_t false_margin = pow(exp(1), log(.5)/n);
    char result = 1;

    for (int x = 0; x < n; ++x){
        if(rand() / (real_t)(RAND_MAX) < false_margin){
            a[x] = 1;
        }
        else{
            a[x] = 0;
        }
    }


    #pragma acc data copyin(a[0:n])
    {
        #pragma acc kernels loop reduction(&&:result)
        for (int x = 0; x < n; ++x){
            result = result && a[x];
        }
    }

    char found = 0;
    for (int x = 0; x < n; ++x){
        if (a[x] == 0){
            found = 1;
            break;
        }
    }
    if (found == result){
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
