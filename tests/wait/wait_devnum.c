#include "acc_testsuite.h"
#include <stdlib.h>
#include <math.h>

#ifndef T1
//T1:runtime,data,async,multi-gpu,V:3.0-3.3
int test1(){

    // single gpu test 

    int err = 0;
    real_t *a = (real_t *)malloc(n * sizeof(real_t));
    real_t *b = (real_t *)malloc(n * sizeof(real_t));
    real_t *c = (real_t *)malloc(n * sizeof(real_t));

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = 0;
        c[x] = 2 * a[x];
    }

    acc_set_device_num(0, acc_device_nvidia);

    #pragma acc data copy(a[0:n], b[0:n])
    {
        #pragma acc parallel loop async(1)
        for(int i = 0; i < n; ++i){
            a[i] *= 2;
        }

        #pragma acc wait(devnum:0, queues:1) async(2)
        #pragma acc parallel loop async(2)
        for(int i = 0; i < n; ++i){
            b[i] = a[i];
        }

        #pragma acc wait
    }

    for(int x = 0; x < n; ++x){
        if(fabs(b[x] - c[x]) > PRECISION){
            err++;
            break;
        }
    }

    free(a);
    free(b);
    free(c);

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
