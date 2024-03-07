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

#pragma acc routine vector
void inner(real_t arr1, int n)
{
    #pragma acc loop vector
    for(int i = 0; i < n; i++)
    {
        arr1 = arr1 + 1;
    }
}

#pragma acc routine gang(dim:1)
void outer(real_t arr1[n], int n)
{
    #pragma acc loop gang(dim:1)
    for(int i = 0; i < n; i++)
    {
        inner(arr1[i], n);
    }
}

#ifndef T3
//T3:parallel,gang,dim,routine,V:3.3
int test3() {
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
    for(int i = 0; i < n; i++)
    {
        outer(arr1[i], n);
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

#pragma acc routine vector
void inner_3D(real_t arr1, int n)
{
    #pragma acc loop vector
    for(int i = 0; i < n; i++)
    {
        arr1 = arr1 + 1;
    }
}

#pragma acc_routine gang(dim:1)
void middle(real_t arr1[n], int n)
{
    #pragma acc loop gang(dim:1)
    for(int i = 0; i < n; i++)
    {
        inner_3D(arr1[i], n);
    }
}

#pragma acc routine gang(dim:2)
void outer_3D(real_t arr1[n][n], int n)
{
    #pragma acc loop gang(dim:1)
    for(int i = 0; i < n; i++)
    {
        middle(arr1[i], n);
    }
}

#ifndef T4
//T4:parallel,gang,dim,routine,V:3.3
int test4() {
    int err = 0;
    srand(SEED); 

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

    #pragma acc parallel loop num_gangs(n,n,n) gang(dim:3)
    for(int i = 0; i < n; i++)
    {
        outer_3D(arr1[i], n);
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
#ifndef T3
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x){
        failed = failed + test3();
    }
    if (failed != 0){
        failcode = failcode + (1 << 2);
    }
#endif
#ifndef T4
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x){
        failed = failed + test4();
    }
    if (failed != 0){
        failcode = failcode + (1 << 3);
    }
#endif
    return failcode;
}
