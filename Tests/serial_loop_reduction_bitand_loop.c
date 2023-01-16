#include "acc_testsuite.h"
#ifndef T1
//T1:serial,loop,reduction,combined-constructs,V:2.6-3.2
int test1(){
    int err = 0;
    srand(SEED);
    unsigned int * a = (unsigned int *)malloc(10 * n * sizeof(unsigned int));
    unsigned int * b = (unsigned int *)malloc(10 * n * sizeof(unsigned int));
    unsigned int * b_copy = (unsigned int *)malloc(10 * n * sizeof(unsigned int));
    unsigned int * c = (unsigned int *)malloc(10 * sizeof(unsigned int));
    unsigned int* host_c = (unsigned int *)malloc(10 * sizeof(unsigned int));

    real_t false_margin = pow(exp(1), log(.5)/n);
    unsigned int temp = 1;

    for (int x = 0; x < 10 * n; ++x){
        b[x] = (unsigned int) rand() / (real_t)(RAND_MAX / 1000);
        b_copy[x] = b[x];
        for (int y = 0; y < 16; ++y){
            if (rand() / (real_t) RAND_MAX < false_margin){
                for (int z = 0; z < y; ++z){
                    temp *= 2;
                }
                a[x] += temp;
                temp = 1;
            }
        }
    }
    #pragma acc data copyin(a[0:10 * n]) copy(b[0:10 * n], c[0:10])
    {
        #pragma acc serial
	{   
		#pragma acc loop gang private(temp)
        	for (int y = 0; y < 10; ++y){
            		temp = a[y * n];
            		#pragma acc loop worker reduction(&:temp)
           		for (int x = 1; x < n; ++x){
               		temp = temp & a[y * n + x];
            		}
            	
			c[y] = temp;
            	
			#pragma acc loop worker
            		for (int x = 0; x < n; ++x){
	       			b[y * n + x] = b[y * n + x] + c[y];
            		}
        	}
    	}
    }
    for (int x = 0; x < 10; ++x){
        host_c[x] = a[x * n];
        for (int y = 1; y < n; ++y){
            host_c[x] = host_c[x] & a[x * n + y];
        }
        if (host_c[x] != c[x]){
          err += 1;
        }
    }

    for (int x = 0; x < 10; ++x){
        for (int y = 0; y < n; ++y){
            if (b[x * n + y] != b_copy[x * n + y] + c[x]){
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
