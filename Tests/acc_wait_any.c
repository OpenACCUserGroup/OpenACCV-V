#include "acc_testsuite.h"
#include <stdio.h>

#ifndef T1
//T1:runtime,async,wait,V:3.2-3.3
int test1() {
    int err = 0;
    srand(SEED);

    real_t * a = (real_t *)malloc(n * sizeof(real_t));
    real_t * b = (real_t *)malloc(n * sizeof(real_t));
    real_t * c = (real_t *)malloc(n * sizeof(real_t));
    real_t * d = (real_t *)malloc(n * sizeof(real_t));

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0;
        d[x] = 0;
    }
    

    for (int x = 0; x < n; ++x) {
        d[x] = a[x] * 2 + b[x] * 3 + 1;
    }

    #pragma acc data copy(a[0:n], b[0:n], c[0:n])
    {
        int queues[3];
        for (int i = 0; i < 3; i++)
        {
            queues[i] = i;

            #pragma acc parallel loop async(i)
            for(int k = 0; k < n; k++) {
                if (i == 0) {
                    a[k] *= 2;  
                } else if (i == 1) {
                    b[k] *= 3;  
                } else {
                    c[k] = 1;  
                }
            }
        }

        int next;
        while ((next = acc_wait_any(3, queues)) >= 0)
        {
            queues[next] = acc_async_sync;
            
            #pragma acc parallel loop async(2)
            for(int j = 0; j < n; j++)
            {
                if (next == 0) {
                    c[j] += a[j];  
                } else if (next == 1) {
                    c[j] += b[j];  
                }
            }
        }
    }

    for (int x = 0; x < n; ++x){
        if (fabs(c[x] - d[x]) > PRECISION){
            err = 1;
            break;
        }
    }

    free(a);
    free(b);
    free(c);
    free(d);

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