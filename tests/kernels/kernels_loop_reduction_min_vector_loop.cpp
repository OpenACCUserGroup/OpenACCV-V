#include "acc_testsuite.h"
#ifndef T1
//T1:kernels,loop,reduction,combined-constructs,V:1.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t * a = new real_t[10 * n];
    real_t * b = new real_t[10 * n];
    real_t * min = new real_t[10];
    real_t temp = 100;
    real_t temp_min;

    for (int x = 0; x < 10 * n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX/10);
        b[x] = rand() / (real_t)(RAND_MAX/10);
    }

    #pragma acc data copyin(a[0:10*n], b[0:10*n]) copy(min[0:10])
    {
        #pragma acc kernels loop gang private(temp)
        for (int x = 0; x < 10; ++x){
            temp = 100;
            #pragma acc loop vector reduction(min:temp)
            for (int y = 0; y < n; ++y){
                temp = fmin(a[x * n + y] * b[x * n + y], temp);
            }
            min[x] = temp;
        }
    }
    for (int x = 0; x < 10; ++x){
        temp_min = 100;
        for (int y = 0; y < n; ++y){
            if (temp_min > a[x * n + y] * b[x * n + y]){
                temp_min = a[x * n + y] * b[x * n + y];
            }
        }
        if (fabs(temp_min - min[x]) > PRECISION){
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
