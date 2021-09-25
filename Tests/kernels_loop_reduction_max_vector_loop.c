#include "acc_testsuite.h"
#ifndef T1
//T1:kernels,loop,reduction,combined-constructs,V:1.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t * a = (real_t *)malloc(10 * n * sizeof(real_t));
    real_t * b = (real_t *)malloc(10 * n * sizeof(real_t));
    real_t * max = (real_t *)malloc(10 * sizeof(real_t));
    real_t temp = 0;
    real_t temp_max;

    for (int x = 0; x < 10 * n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX/10);
        b[x] = rand() / (real_t)(RAND_MAX/10);
    }
    #pragma acc data copyin(a[0:10*n], b[0:10*n]) copy(max[0:10])
    {
        #pragma acc kernels loop private(temp)
        for (int x = 0; x < 10; ++x){
            temp = 0;
            #pragma acc loop vector reduction(max:temp)
            for (int y = 0; y < n; ++y){
                temp = fmax(a[x * n + y] * b[x * n + y], temp);
            }
            max[x] = temp;
        }
    }
    for (int x = 0; x < 10; ++x){
        temp_max = 0;
        for (int y = 0; y < n; ++y){
            if (temp_max < a[x * n + y] * b[x * n + y]){
                temp_max = a[x * n + y] * b[x * n + y];
            }
        }
        if (fabs(temp_max - max[x]) > PRECISION){
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
