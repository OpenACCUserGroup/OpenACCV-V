#include "acc_testsuite.h"
#ifndef T1
//T1:atomic,construct-independent,V:2.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    unsigned int *a = (unsigned int *)malloc(3 * n * sizeof(int));
    unsigned int *b = (unsigned int *)malloc(n * sizeof(int));
    int orders[18] = {1, 2, 0, 1, 0, 2, 2, 1, 0, 2, 0, 1, 0, 1, 2, 0, 2, 1};
    int result;

    for (int x = 0; x < n; ++x){
        for (int y = 0; y < 3; ++y){
            a[x * 3 + y] = (int) (rand() / (unsigned int) (RAND_MAX / 4));
        }
        b[x] = 0;
    }

    #pragma acc data copyin(a[0:3 * n]) copy(b[0:n])
    {
        #pragma acc parallel
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                #pragma acc loop independent
                for (int y = 0; y < 3; ++y){
                    #pragma acc atomic update
                        b[x] = a[x * 3 + y] >> b[x];
                }
            }
        }
    }

    for (int x = 0; x < n; ++x){
        for (int y = 0; y < 6; ++y){
            result = 0;
            for (int z = 0; z < 3; ++z){
                result = a[x * 3 + orders[y * 3 + z]] >> result;
            }
            if (result == b[x]){
                break;
            }
        }
        if (result != b[x]){
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
