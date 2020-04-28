#include "acc_testsuite.h"
#ifndef T1
//T1:kernels,loop,reduction,combined-constructs,V:1.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    unsigned int * a = (unsigned int *)malloc(10 * n * sizeof(unsigned int));
    unsigned int * b = (unsigned int *)malloc(10 * n * sizeof(unsigned int));
    unsigned int * b_copy = (unsigned int *)malloc(10 * n * sizeof(unsigned int));
    unsigned int * c = (unsigned int *)malloc(10 * sizeof(unsigned int));

    real_t false_margin = pow(exp(1), log(.5)/n);
    unsigned int temp = 1;
    unsigned int host_c;

    for (int x = 0; x < 10; ++x){
        for (int y = 0; y < n; ++y){
            b[x * n + y] = (unsigned int) rand() / (real_t)(RAND_MAX / 1000);
            b_copy[x * n + y] = b[x * n + y];
            for (int z = 0; z < 16; ++z){
                if (rand() / (real_t) RAND_MAX > false_margin){
                    temp = 1;
                    for (int i = 0; i < z; ++i){
                        temp = temp * 2;
                    }
                    a[x * n + y] += temp;
                }
            }
        }
    }

    #pragma acc data copyin(a[0:10*n]) copy(b[0:10*n], c[0:10])
    {
        #pragma acc kernels loop gang private(temp)
        for (int x = 0; x < 10; ++x){
            temp = 0;
            #pragma acc loop worker reduction(|:temp)
            for (int y = 0; y < n; ++y){
                temp = temp | a[x * n + y];
            }
            c[x] = temp;
            #pragma acc loop worker
            for (int y = 0; y < n; ++y){
                b[x * n + y] = b[x * n + y] + c[x];
            }
        }
    }
    for (int x = 0; x < 10; ++x){
        host_c = a[x * n];
        for (int y = 1; y < n; ++y){
            host_c = host_c | a[x * n + y];
        }
        if (host_c != c[x]){
            err += 1;
        }
        for (int y = 0; y < n; ++y){
            if (b[x * n + y] != b_copy[x * n + y] + host_c){
                err += 1;
            }
        }
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
