#include "acc_testsuite.h"
#ifndef T1
//T1:serial,loop,async,combined-constructs,V:2.6-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t * a = (real_t *)malloc(10 * n * sizeof(real_t));
    real_t * b = (real_t *)malloc(10 * n * sizeof(real_t));
    real_t * c = (real_t *)malloc(10 * n * sizeof(real_t));
    real_t * d = (real_t *)malloc(10 * n * sizeof(real_t));
		int * errors = (int *)malloc(10 * sizeof(int));

		for (int x = 0; x < 10; ++x){
			errors[x] = 0;
		}

    for (int x = 0; x < 10 * n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0.0;
        d[x] = a[x] + b[x];
    }

    #pragma acc data copyin(a[0:10*n], b[0:10*n], c[0:10*n], d[0:10*n]) copy(errors[0:10])
    {
        for (int x = 0; x < 10; ++x){
            #pragma acc serial loop async(x)
            for (int y = 0; y < n; ++y){
                c[x * n + y] = a[x * n + y] + b[x * n + y];
            }
            #pragma acc serial loop async(x)
            for (int y = 0; y < n; ++y){
                if(c[x * n + y] - d[x * n + y] > PRECISION || d[x * n + y] - c[x * n + y] > PRECISION){
                    errors[x] += 1;
                }
            }
        }
				#pragma acc wait
    }

    for (int x = 0; x < 10; ++x){
        err += errors[x];
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
