#include "acc_testsuite.h"

#pragma acc routine vector
void inner(real_t *arr1, int n) {
        
	#pragma acc loop vector
        for ( int k = 0; k < n; k++) {
             arr1[k] = arr1[k] + 1;
    }
}

#pragma acc routine gang(dim:1)
void outer(real_t **arr1, int n) {
    
    #pragma acc loop gang(dim:1)
    for ( int j = 0; j < n; j++) {
        
        inner (arr1[j], n);
    }
}

#ifndef T1
//T1:parallel,gang,dim,loop,V:3.3
int test1(){
    int err = 0;
    srand(SEED); 

    real_t arr1[n][n];
    real_t arr2[n][n];

    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            arr1[i][j] = rand() / (real_t)(RAND_MAX / 10);
            arr2[i][j] = arr1[i][j] + 1;
        }
    }

    #pragma acc parallel loop num_gangs(n,n) gang(dim:2)
    for (int i = 0; i < n; i++) {
        outer(arr1, n);
    }

    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            if (fabs(arr1[i][j] - arr2[i][j]) > PRECISION)  {
                err = 1;
            }
        }
    }

    return err;
}
#endif

int main() {
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
