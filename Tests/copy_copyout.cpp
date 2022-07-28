#include "acc_testsuite.h"

using namespace std;

#ifndef T1
int test1(){
    int err = 0;
    real_t test = 0;
    #pragma acc parallel loop copy(test) copyout(test) reduction(+:test)
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
int test2(){
    int err = 0;
    real_t *test = (real_t *)malloc(n * sizeof(real_t));

    for(int x = 0; x < n; ++x){
        test[x] = 1.0;
    }

   #pragma acc parallel loop copy(test[0:n]) copyout(test[0:n])
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
#ifndef T3
int test3(){
    int err = 0;
    data_container<real_t> a = *(new data_container<real_t>(n));

    for(int x = 0; x < n; ++x){
	a.data[x] = 1.0;
    }

    #pragma acc parallel loop copy(a, a.data[0:n]) copyout(a, a.data[0:n])
    for(int x = 0; x < n; ++x){
	a.data[x] += 1.0;
    }

    for(int x = 0; x < n; ++x){
	if(fabs(a.data[x] - 2.0) > PRECISION){
	    err ++;
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
#ifndef T3
    failed = 0;
    for( int x = 0; x < NUM_TEST_CALLS; ++x){
        failed += test3();
    }
    if(failed){
        failcode += (1 << 2);
    }
#endif
    return failcode;
}

