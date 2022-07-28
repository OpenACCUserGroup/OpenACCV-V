#include "acc_testsuite.h"
#ifndef T1
//T1:serial,loop,reduction,combined-constructs,V:2.6-3.2
int test1(){
    int err = 0;
    srand(SEED);
    real_t * a = (real_t *)malloc(10 * n * sizeof(real_t));
    real_t * b = (real_t *)malloc(10 * n * sizeof(real_t));
    real_t * c = (real_t *)malloc(10 * n * sizeof(real_t));
    real_t * maximum = (real_t *)malloc(10 * sizeof(real_t));
    real_t temp = 0;

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX/10);
        b[x] = rand() / (real_t)(RAND_MAX/10);
        c[x] = 0.0;
    }
    for (int x = 0; x < 10; ++x){
        maximum[x] = 0;
    }

    #pragma acc data copyin(a[0:10*n], b[0:10*n]) copy(c[0:10*n], maximum[0:10])
    {
        #pragma acc serial loop private(temp)
        for (int x = 0; x < 10; ++x){
            temp = 0;
            #pragma acc loop worker reduction(max:temp)
            for (int y = 0; y < n; ++y){
                temp = fmax(a[x * n + y] * b[x * n + y], temp);
            }
            maximum[x] = temp;
            #pragma acc loop worker
            for (int y = 0; y < n; ++y){
                c[x * n + y] = (a[x * n + y] * b[x * n + y]) / maximum[x];
            }
        }
    }
    for (int x = 0; x < 10; ++x){
        temp = 0;
        for (int y = 0; y < n; ++y){
            temp = fmax(temp, a[x * n + y] * b[x * n + y]);
        }
        if (fabs(temp - maximum[x]) > PRECISION){
            err += 1;
        }
        for (int y = 0; y < n; ++y){
            if (c[x * n + y] > 1){
                err += 1;
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
