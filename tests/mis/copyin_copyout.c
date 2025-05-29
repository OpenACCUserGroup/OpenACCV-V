#include "acc_testsuite.h"
#ifndef T1
//T1:, V:1.0-2.7
int test1(){
    int err = 0;
    real_t test = 0;
    #pragma acc parallel loop copyin(test) copyout(test) reduction(+:test)
    for( int x = 0; x <n; ++x){
        test += 1;
    }

    if(fabs(test - n) > PRECISION){
        err++;
    }

    return err;
}
#endif

#ifndef T2
//T2: , V:1.0-2.7
int test2(){
    int err = 0;
    real_t *test = (real_t *)malloc(n * sizeof(real_t));

    for(int x = 0; x < n; ++x){
        test[x] = 1.0;
    }

   #pragma acc parallel loop copyin(test[0:n]) copyout(test[0:n])
   for(int x = 0; x < n; ++x){
        test[x] += 1.0;
   }

   for(int x = 0; x < n; ++x){
        if(fabs(test[x] - 2.0) > PRECISION){
            err++;
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
    for( int x = 0; x < NUM_TEST_CALLS; ++x){
	failed += test1();
    }
    if(failed){
	failcode += (1 << 0);
    }
#endif
#ifndef T2
    failed = 0;
    for( int x = 0; x < NUM_TEST_CALLS; ++x){
        failed += test2();
    }
    if(failed){
        failcode += (1 << 1);
    }
#endif
    return failcode;
}

