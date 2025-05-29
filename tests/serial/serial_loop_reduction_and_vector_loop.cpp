#include "acc_testsuite.h"
#ifndef T1
//T1:serial,loop,reduction,combined-constructs,V:2.6-2.7
int test1(){
    int err = 0;
    srand(SEED);
    char * a = new char[10 * n];
    char * b = new char[10];
    char * has_false = new char[10];
    char temp = 1;
    real_t false_margin = pow(exp(1), log(.5)/n);

    for (int x = 0; x < 10; ++x){
        has_false[x] = 0;
    }
    for (int x = 0; x < 10 * n; ++x){
        if(rand() / (real_t)(RAND_MAX) < false_margin){
            a[x] = 1;
        }
        else{
            a[x] = 0;
            has_false[x/n] = 1;
        }
    }
    #pragma acc data copyin(a[0:10*n]) copy(b[0:10])
    {
        #pragma acc serial loop private(temp)
        for (int x = 0; x < 10; ++x){
            temp = 1;
            #pragma acc loop vector reduction(&&:temp)
            for (int y = 0; y < n; ++y){
                temp = temp && a[x * n + y];
            }
            b[x] = temp;
        }
    }
    for (int x = 0; x < 10; ++x){
        if (has_false[x] == b[x]){
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
