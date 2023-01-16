#include "acc_testsuite.h"
#ifndef T1
//T1:serial,loop,reduction,combined-constructs,V:2.6-3.2
int test1(){
    int err = 0;
    srand(SEED);
    real_t * a = (real_t *)malloc(10 * n * sizeof(real_t));
    real_t * b = (real_t *)malloc(10 * n * sizeof(real_t));
    real_t * c = (real_t *)malloc(10 * n * sizeof(real_t));
    real_t * d = (real_t *)malloc(10 * n * sizeof(real_t));
    real_t avg = 0.0;
    real_t rolling_total = 0;

    for (int x = 0; x < 10 * n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
    }

    #pragma acc data copyin(a[0:10*n], b[0:10*n]) create(c[0:10*n]) copyout(d[0:10*n])
    {
        #pragma acc serial 
	{    
	#pragma acc loop gang private(avg)
        for (int x = 0; x < 10; ++x){
            avg = 0;
            #pragma acc loop worker reduction(+:avg)
            for (int y = 0; y < n; ++y){
                avg += c[x * n + y] = a[x * n + y] + b[x * n + y];
            }
            avg = avg / n;
            #pragma acc loop worker
            for (int y = 0; y < n; ++y){
                d[x * n + y] = c[x * n + y] - avg;
            }
        }
	}
    }


    for (int x = 0; x < 10; ++x){
        rolling_total = 0;
        for (int y = 0; y < n; ++y){
            rolling_total += a[x * n + y] + b[x * n + y];
        }
        rolling_total = rolling_total / n;
        for (int y = 0; y < n; ++y){
            if (fabs(d[x * n + y] - ((a[x * n + y] + b[x * n + y]) - rolling_total)) > PRECISION){
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
