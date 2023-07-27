#include "acc_testsuite.h"

#ifndef T1
//T1:kernels,data,data-region,V:2.0-3.3
int test1(){
	int err = 0;
	srand(SEED);
	int temp = rand()/ (real_t)(RAND_MAX / 10);

	#pragma acc kernels
	for ( int x = 0; x < n; ++x){
		temp += temp;
	}

	if(temp > PRECISION){
		err = 1;
	}
	return err;
}
#endif
#ifndef T2
//T1:kernels,data,data-region,V:2.0-3.3
int test2(){
	int err = 0;
	srand(SEED);

	real_t *test_array = new real_t[n];
	real_t *host_array = new real_t[n];

	for( int x = 0; x < n; ++x){
		test_array[x] = rand()/(real_t)(RAND_MAX/ 10);
		host_array[x] = test_array[x];
	}

	#pragma acc kernels
	for(int x = 0; x < n; ++x){
		test_array[x] += test_array[x];
	}

	for(int x = 0; x < n; ++x){
                if(fabs(host_array[x]*2 - test_array[x]) > PRECISION){
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
	for(int x = 0; x < NUM_TEST_CALLS; ++x){
		failed += test1();
	}
	if(failed){
		failcode += ( 1 << 0);
	}
#endif
#ifndef T2
        failed = 0;
        for(int x = 0; x < NUM_TEST_CALLS; ++x){
                failed += test2();
        }
        if(failed != 0){
                failcode += ( 1 << 1);
        }
#endif
	return failcode;
}
