#include "acc_testsuite.h"
#ifndef T1
//T1:atomic,construct-independent,V:2.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t *a = new real_t[n];
    real_t *b = new real_t[n];
    int *c = new int[n];
    int *distribution = new int[10];
    int *distribution_comparison = new int[10];

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
    }
    for (int x = 0; x < 10; ++x){
        distribution[x] = 0;
        distribution_comparison[x] = 0;
    }

    #pragma acc data copyin(a[0:n], b[0:n]) copy(distribution[0:10]) copyout(c[0:n])
    {
        #pragma acc parallel
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                #pragma acc atomic capture
                    c[x] = distribution[(int) (a[x]*b[x]/10)]++;
            }
        }
    }

    for (int x = 0; x < n; ++x){
        distribution_comparison[(int) (a[x]*b[x]/10)]++;
    }
    for (int x = 0; x < 10; ++x){
        if (distribution_comparison[x] != distribution[x]){
            err += 1;
            break;
        }
    }

    bool found = false;
    for (int x = 0; x < 10; ++x){
        for (int y = 0; y < distribution_comparison[x]; ++y){
            for (int z = 0; z < n; ++z){
                if (c[z] == y && (int) (a[z]*b[z]/10) == x){
                    found = true;
                    break;
                }
            }
            if (!found){
                err++;
            }
            found = false;
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
