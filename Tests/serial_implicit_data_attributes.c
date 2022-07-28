#include "acc_testsuite.h"

#ifndef T1
//copy cluase test with serial default(none) and reduction with scalar variables
int test1(){
	int err = 0;
	srand(SEED);
	int temp = rand()/(real_t)(RAND_MAX / 10);
	#pragma acc serial default(none) reduction(+:temp)
	for(int x = 0; x < n; ++x){
		temp += temp;
	}
	if(temp > PRECISION){
		err = 1;
	}
	return err;
}
#endif
#ifndef T2
//copy clause test with serial loop and reduction with scalar
int test2(){
	int err = 0;
	srand(SEED);
	int temp = rand()/(real_t)(RAND_MAX / 10);
	#pragma acc serial loop reduction(+:temp)
	for(int x = 0; x < n; ++x){
		temp += temp;
	}
	if(temp > PRECISION){
		err = 1;
	}
	return err;
}
#endif
#ifndef T3
//firstprivate test with only serial and reduction with scalar variables
int test3(){
	int err = 0;
	srand(SEED);
	int host = rand()/(real_t)(RAND_MAX/10);
	int device = host;
	#pragma acc serial reduction(+:device)
	for( int x = 0; x < n; ++x){
		device += device;
	}

	if( fabs(host - device) > PRECISION ){
		err = 1;
	}
	return err;
}
#endif
#ifndef T4
int test4(){
	int err = 0;
	srand(SEED);

	real_t *host_array = (real_t *) malloc ( n *sizeof(real_t));
        real_t *device_array = (real_t *) malloc ( n *sizeof(real_t));

	for( int x = 0; x < n; ++x){
                host_array[x] = rand()/ (real_t)(RAND_MAX/10);
                device_array[x] = host_array[x];
        }
	
	#pragma acc serial loop
	for ( int x = 0; x < n; ++x){
		device_array[x] += device_array[x];
	}
	
	for( int x = 0; x < n; ++x){
                if(fabs(host_array[x]*2 - device_array[x]) > PRECISION){
                        err = 1;
                }
        }
        free(host_array);
        free(device_array);
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
#ifndef T4
        failed = 0;
        for( int x = 0; x < NUM_TEST_CALLS; ++x){
                failed += test4();
        }
        if(failed){
                failcode += (1 << 3);
        }
#endif
	return failcode;
}
