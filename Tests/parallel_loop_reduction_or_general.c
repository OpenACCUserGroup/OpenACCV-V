#include "acc_testsuite.h"
#ifndef T1
//T1:parallel,loop,reduction,combined-constructs,V:1.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    char * a = (char *)malloc(n * sizeof(char));
    real_t false_margin = pow(exp(1), log(.5)/n);
    char result = 0;
    char found = 0;

    for (int x = 0; x < n; ++x){
        if(rand() / (real_t)(RAND_MAX) > false_margin){
            a[x] = 1;
        }
        else{
            a[x] = 0;
        }
    }


    #pragma acc data copyin(a[0:n])
    {
        #pragma acc parallel loop reduction(||:result)
        for (int x = 0; x < n; ++x){
            result = result || a[x];
        }
    }

    for (int x = 0; x < n; ++x){
        if (a[x] == 1){
            found = 1;
        }
    }
    if (found != result){
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
    char* a = (char *)malloc(5 * n * sizeof(char));
    char result[5];
    char host_result[5];
    real_t false_margin = pow(exp(1), log(.5)/n);

    for (int x = 0; x < 5; ++x) {
        result[x] = 0;
        host_result[x] = 0;
    }

    for (int x = 0; x < 5 * n; ++x) {
        if (rand() / (real_t)(RAND_MAX) > false_margin) {
            a[x] = 1;
        }
        else {
            a[x] = 0;
        }
    }

    #pragma acc data copyin(a[0:5*n])
    {
        #pragma acc parallel loop reduction(||:result)
        for (int x = 0; x < 5 * n; ++x) {
            result[x%5] = result[x%5] || a[x];
        }
    }

    for (int x = 0; x < 5 * n; ++x) {
        host_result[x%5] = host_result[x%5] || a[x];
    }
    
    for (int x = 0; x < 5; ++x) {
        if (host_result[x] != result[x]) {
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
