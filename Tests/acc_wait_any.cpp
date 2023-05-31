#include "acc_testsuite.h"
#ifndef T1
//T1:runtime,async,wait,V:3.2-3.3
int test1() {

    int err = 0;
    srand(SEED);

    real_t * a = new real_t[n];
    real_t * b = new real_t[n];
    real_t * c = new real_t[n];


    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = (a[x] + 1) * 2 + (b[x] + 1) * 2;
    }
    
    real_t *list[3] = {a, b, c};

    #pragma acc data copyin(list[0:3][0:n])
    {
        int queues[10];
        for (int i = 0; i < 2; i++)
        {
            // Do some unbalanced operation on several queues
            #pragma acc enter data copyin(list[i]) async(i)
            
            // Put the queue number in the queues list
            queues[i] = i;

            #pragma acc parallel loop async(i)
            for(int k = 0; k < n; k++) {
                list[i][k]++;
            }
        }
        int next;
        // Look for queue that is ready to process
        while ((next = acc_wait_any(3, queues)) >= 0)
        {
            // Remove this queue from consideration next time around
            queues[next] = acc_async_sync;
            
            // Process work dependent on above
            #pragma acc kernels
            {
                for(int i = 0; i < n; i++)
                {
                    list[next][i] = list[next][i] * 2;
                }
            }
        }
    }

    for (int x = 0; x < n; ++x){
        if (fabs(c[x] - (a[x] + b[x])) > PRECISION){
            err = 1;
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
