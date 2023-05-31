#include "acc_testsuite.h"

#ifndef T1
//T1:serial,V:1.0-2.7
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
//T2:serial,V:1.0-2.7
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
//T3:serial, firstprivate, V:1.0-2.7
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
//T4:serial, host_dataV:1.0-2.7
int test4(){
	int err = 0;
	srand(SEED);

	real_t *host_array = new real_t[n];
        real_t *device_array = new real_t[n];

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
        delete[] host_array;
        delete[] device_array;
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
