#include "acc_testsuite.h"
#ifndef T1
//T1:parallel,wait,async,V:2.0-3.2
int test1(){
    int err = 0;
    srand(SEED);
    real_t *a = new real_t[n];
    real_t *b = new real_t[n];

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = 0.0;
    }

    #pragma acc data copyin(a[0:n], b[0:n])
    {
	#pragma acc parallel loop present(a[0:n], b[0:n]) async(1)
        for (int x = 0; x < n; ++x){
	    b[x] = a[x];
        }
	#pragma acc update host(b[0:n]) wait(1)
    }
    for (int x = 0; x < n; ++x){
        if (fabs(a[x] - b[x]) > PRECISION){
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
        failed += test1();
    }
    if (failed){
        failcode +=  (1 << 0);
    }
#endif
    return failcode;
}
