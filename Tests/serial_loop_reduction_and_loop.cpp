#include "acc_testsuite.h"
#ifndef T1
//T1:serial,loop,reduction,combined-constructs,V:2.6-3.2
int test1(){
    int err = 0;
    srand(SEED);
    char * a = new char[10 * n];
    char * a_copy = new char[10 * n];
    char * has_false = new char[10];
    real_t false_margin = pow(exp(1), log(.5)/n);
    char temp = 1;

    for (int x = 0; x < 10; ++x){
        has_false[x] = 0;
    }

    for (int x = 0; x < 10; ++x){
        for (int y = 0; y < n; ++y){
            if (rand() / (real_t)(RAND_MAX) < false_margin){
                a[x * n + y] = 1;
                a_copy[x * n + y] = 1;
            }
            else {
                a[x * n + y] = 0;
                a_copy[x * n + y] = 0;
                has_false[x] = 1;
            }
        }
    }

    #pragma acc data copy(a[0:10*n])
    {
        #pragma acc serial
	{   
	#pragma acc loop gang private(temp)
        for (int x = 0; x < 10; ++x){
            temp = 1;
            #pragma acc loop worker reduction(&&:temp)
            for (int y = 0; y < n; ++y){
                temp = temp && a[x * n + y];
            }
            #pragma acc loop worker
            for (int y = 0; y < n; ++y){
                if(temp == 1){
                    if  (a[x * n + y] == 1){
                        a[x * n + y] = 0;
                    }
                    else {
                        a[x * n + y] = 1;
                    }
                }
            }
        }
	}
    }


    for (int x = 0; x < 10; ++x){
        for (int y = 0; y < n; ++y){
            if (has_false[x] == 1 && a[x * n + y] != a_copy[x * n + y]){
                err = 1;
            }
            else if (has_false[x] == 0 && a[x * n + y] == a_copy[x * n + y]){
                err = 1;
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
