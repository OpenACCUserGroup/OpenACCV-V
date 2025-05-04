#include "acc_testsuite.h"

#ifndef T1
//T1:parallel,gang,dim,V:3.3
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

    #pragma acc parallel num_gangs(n,n) 
    #pragma acc loop gang(dim:2)
    for (int i = 0; i < n; i++)
    {    
        #pragma acc loop gang(dim:1)
        for (int j = 0; j < n; j++)
        {
            arr1[i][j] = arr1[i][j] + 1;
        }
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

#ifndef T2
//T2:parallel,gang,dim,V:3.3
int test2(){
    int err = 0;
    srand(SEED); 

    n = 8;

    real_t arr1[n][n][n];
    real_t arr2[n][n][n];

    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            for(int k = 0; k < n; k++) {
                arr1[i][j][k] = rand() / (real_t)(RAND_MAX / 10);
                arr2[i][j][k] = arr1[i][j][k] + 1;
            }
        }
    }

    #pragma acc parallel num_gangs(n,n,n) 
    #pragma acc loop gang(dim:3)
    for (int i = 0; i < n; i++)
    {
        #pragma acc loop gang(dim:2)
        for (int j = 0; j < n; j++)
        {
            #pragma acc loop gang(dim:1)
            for (int k = 0; k < n; k++)
            {
                arr1[i][j][k] = arr1[i][j][k] + 1;
            }
        }
    }

    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            for(int k = 0; k < n; k++) {
                if (fabs(arr1[i][j][k] - arr2[i][j][k]) > PRECISION)  {
                    err = 1;
                }
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
#ifndef T2
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x){
        failed = failed + test2();
    }
    if (failed != 0){
        failcode = failcode + (1 << 1);
    }
#endif
    return failcode;
}

