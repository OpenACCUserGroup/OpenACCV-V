#include "acc_testsuite.h"
#ifndef T1
//T1:parallel,runtime,V:2.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t * a = (real_t *)malloc(n * sizeof(real_t));
    real_t * b;

    for (int x = 0; x < n; ++x){
        a[x] = 0;
    }

    #pragma acc enter data copyin(a[0:n])
    b = (real_t *) acc_deviceptr(a);
    if (b == NULL){
        err = 1;
    }
    else{
        #pragma acc parallel deviceptr(b)
        {
            #pragma acc loop
            for (int x = 0; x < (int) n/2; ++x){
                b[x] = 1;
            }
        }
        #pragma acc exit data copyout(a[0:n])
    }
    for (int x = 0; x < n; ++x){
        if (x < (int) n/2){
            if (fabs(a[x] - 1) > PRECISION){
                err = 1;
            }
        }
        else {
            if (fabs(a[x]) > PRECISION){
                err = 1;
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
