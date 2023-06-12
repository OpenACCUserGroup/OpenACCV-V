#include "acc_testsuite.h"
#ifndef T1
//T1:atomic,construct-independent,V:2.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    unsigned int *a = (unsigned int *)malloc(n * sizeof(int));
    unsigned int *b = (unsigned int *)malloc(n * sizeof(int));

    for (int x = 0; x < n; ++x){
        a[x] = 1;
        for (int y = 0; y < 7; ++y){
            if ((rand()/(unsigned int) (RAND_MAX)) > .5){
                b[x] += 1<<y;
            }
        }
    }

    #pragma acc data copyin(b[0:n]) copy(a[0:n])
    {
        #pragma acc parallel
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                #pragma acc loop
                for (int y = 0; y < 7; ++y){
                    if ((b[x]>>y)%2 == 1){
                        #pragma acc atomic update
                            a[x] <<= 1;
                    }
                }
            }
        }
    }

    for (int x = 0; x < n; ++x){
        for (int y = 0; y < 7; ++y){
            if ((b[x]>>y)%2 == 1){
                a[x] >>= 1;
            }
        }
        if (a[x] != 1){
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
    return failcode;
}
