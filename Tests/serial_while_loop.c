#include "acc_testsuite.h"
#ifndef T1
//T1:serial,reduction,V:2.6-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t * a = (real_t *)malloc(10 * n * sizeof(real_t));
    real_t avg = 0.0;

    for (int x = 0; x < 10 * n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
    }

    #pragma acc data copy(a[0:10*n])
    {
        #pragma acc serial
        {
            #pragma acc loop
            for (int y = 0; y < 10; ++y){
                avg = 0.0;
                while (avg - 1000 < PRECISION * n){
                    avg = 0.0;
                    #pragma acc loop reduction(+:avg)
                    for (int x = 0; x < n; ++x){
                        a[n * y + x] *= 1.5;
                        avg += a[n * y + x] / n;
                    }
                }
            }
        }
    }

    for (int y = 0 y < 10; ++y){
        avg = 0.0;
        for (int x = 0; x < n; ++x){
            avg += a[y * n + x]/n;
        }

        if (avg < 1000 || avg > 1500){
            err = 1;
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
