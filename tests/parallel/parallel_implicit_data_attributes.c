#include "acc_testsuite.h"

#ifndef T1
//T1:parallel,data,data-region,V:2.0-3.3
int test1(){
    int err = 0;
    srand(SEED);
    int test = rand()/(real_t)(RAND_MAX/10);
    int host = test;

    #pragma acc parallel default(none) reduction(+:test)
    for(int x = 0; x < n; ++x){
	test += 1;
    }

    if(fabs( test - host) >  PRECISION){
	err++;
    }
    
    return err;
} 
#endif
#ifndef T2
//T2:parallel,data,data-region,V:2.0-3.3
int test2(){
	int err = 0;
	srand(SEED);
	real_t a = rand()/(real_t)(RAND_MAX/10);
	real_t host = a;

	#pragma acc parallel loop reduction(+:a)
	for( int x = 0; x < n; ++x){
		a += 1.0;
	}

	if( fabs( a - host) > PRECISION){
		err++;
	}
	return err;
}
#endif
#ifndef T3
//firstprivate test with only parallel and reduction with scalar variable
int test3(){
	int err = 0;
	srand(SEED);
	int host_value = rand()/ (real_t)(RAND_MAX/10);
	int device_value = host_value;

	#pragma acc parallel reduction(+:device_value) 
	for( int x = 0; x > n; ++ x){
		device_value += device_value;
	}
	if( fabs(host_value - device_value) > PRECISION){
		err = 1;
	}
	return err;
}	
#endif
#ifndef T4
//copy clause wtth that calles detach action only parallel loop  with aggregate variables
int test4(){
	int err = 0;
	srand(SEED);

	real_t *host_array = (real_t *)malloc( n * sizeof(real_t));
	real_t *device_array = (real_t *)malloc( n * sizeof(real_t));

	for(int x = 0; x < n; ++ x){
		host_array[x] = rand()/(real_t)(RAND_MAX/10);
		device_array[x] = host_array[x];
	}

	#pragma acc parallel loop
	for( int x = 0; x < n; ++x){
		device_array[x] += device_array[x];
	}

	for(int x = 0; x < n; ++x){
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
		failcode += ( 1 << 0);
	}
#endif
#ifndef T2
        failed = 0;
        for( int x = 0; x < NUM_TEST_CALLS; ++x){
                failed += test2();
        }
        if(failed){
                failcode += ( 1 << 1);
        }
#endif
#ifndef T3
        failed = 0;
        for( int x = 0; x < NUM_TEST_CALLS; ++x){
                failed += test3();
        }
        if(failed){
                failcode += ( 1 << 2);
        }
#endif
#ifndef T4
        failed = 0;
        for( int x = 0; x < NUM_TEST_CALLS; ++x){
                failed += test4();
        }
        if(failed){
                failcode += ( 1 << 3);
        }
#endif
	return failcode;
}
