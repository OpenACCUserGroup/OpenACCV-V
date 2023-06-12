#include "acc_testsuite.h"
#ifndef T1
//T1:kernels,loop,reduction,combined-constructs,V:1.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    char * a = new char[10 * n];
    char * a_copy = new char[10 * n];
    char * results = new char[10];
    char temp = 0;
    real_t false_margin = pow(exp(1), log(.5)/n);

    for (int x = 0; x < 10 * n; ++x){
        if (rand() / (real_t)(RAND_MAX) > false_margin){
            a[x] = 1;
            a_copy[x] = 1;
        }
        else{
            a[x] = 0;
            a_copy[x] = 0;
        }
    }
    #pragma acc data copy(a[0:10*n])
    {
        #pragma acc kernels loop gang private(temp)
        for (int x = 0; x < 10; ++x){
            temp = 0;
            #pragma acc loop worker reduction(||:temp)
            for (int y = 0; y < n; ++y){
                temp = temp || a[x * n + y];
            }
            results[x] = temp;
            #pragma acc loop worker
            for (int y = 0; y < n; ++y){
                if(results[x] == 1){
                    if (a[x * n + y] == 1){
                        a[x * n + y] = 0;
                    }
                    else{
                        a[x * n + y] = 1;
                    }
                }
            }
        }
    }
    for (int x = 0; x < 10; ++x){
        temp = 0;
        for (int y = 0; y < n; ++y){
            temp = temp || a_copy[x * n + y];
        }
        if (temp != results[x]) {
            err += 1;
        }
        for (int y = 0; y < n; ++y){
            if (temp == 1){
                if (a[x * n + y] == a_copy[x * n + y]){
                    err += 1;
                }
            }
            else {
                if (a[x * n + y] != a_copy[x * n + y]){
                    err += 1;
                }
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
    return failcode;
}
