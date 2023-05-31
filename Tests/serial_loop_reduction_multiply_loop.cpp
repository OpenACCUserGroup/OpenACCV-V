#include "acc_testsuite.h"
#ifndef T1
//T1:serial,loop,reduction,combined-constructs,V:2.6-3.2
int test1(){
    int err = 0;
    srand(SEED);
    real_t * a = new real_t[10 * n];
    real_t * b = new real_t[10 * n];
    real_t * c = new real_t[10 * n];
    real_t * totals = new real_t[10];
    real_t temp;

    for (int x = 0; x < n; ++x){
        a[x] = (999.4 + rand() / (real_t)(RAND_MAX)) / 1000;
        b[x] = (999.4 + rand() / (real_t)(RAND_MAX)) / 1000;
        c[x] = 0.0;
    }


    #pragma acc data copyin(a[0:10*n], b[0:10*n]) copyout(c[0:10*n]) copy(totals[0:10])
    {
        #pragma acc serial 
	{   
	#pragma acc loop gang private(temp)
        for (int x = 0; x < 10; ++x){
            temp = 1;
            #pragma acc loop worker reduction(*:temp)
            for (int y = 0; y < n; ++y){
                temp *= a[x * n + y] + b[x * n + y];
            }
            totals[x] = temp;
            #pragma acc loop worker
            for (int y = 0; y < n; ++y){
                c[x * n + y] = (a[x * n + y] + b[x * n + y]) / totals[x];
            }
        }
	}
    }

    for (int x = 0; x < 10; ++x){
        temp = 1;
        for (int y = 0; y < n; ++y){
            temp *= a[x * n + y] + b[x * n + y];
        }
        if (fabs(temp - totals[x]) > (temp / 2 + totals[x] / 2) * PRECISION){
            err += 1;
        }
        for (int y = 0; y < n; ++y){
            if (fabs(c[x * n + y] - ((a[x * n + y] + b[x * n + y]) / totals[x])) > PRECISION){
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
