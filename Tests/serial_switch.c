#include "acc_testsuite.h"
#ifndef T1
//T1:serial,V:2.6-2.7
int test1(){
    int err = 0;
    srand(SEED);
    int * a = (int *)malloc(n * sizeof(int));
    real_t * b = (real_t *)malloc(n * sizeof(real_t));
    real_t * b_host = (real_t *)malloc(n * sizeof(real_t));
    real_t * c = (real_t *)malloc(n * sizeof(real_t));

    for (int x = 0; x < n; ++x){
        a[x] = floor(rand() / (real_t)(RAND_MAX / 10));
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        b_host[x] = b[x];
        c[x] = 0.0;
    }
    #pragma acc enter data copyin(a[0:n], b[0:n], c[0:n])
    #pragma acc serial present(a[0:n], b[0:n], c[0:n])
    {
        #pragma acc loop
        for (int x = 0; x < n; ++x){
            switch(a[x]){
                case 0:
                    c[x] = b[x] * b[x];
                    break;
                case 1:
                    c[x] = b[x] / b[x];
                    break;
                case 2:
                    b[x] = b[x] / 2;
                default:
                    c[x] = a[x] + b[x];


            }
        }
    }
    #pragma acc exit data delete(a[0:n], b[0:n]) copyout(c[0:n])

    real_t tempc = 0.0;
    for (int x = 0; x < n; ++x){
        switch(a[x]){
            case 0:
                tempc = b_host[x] * b_host[x];
                break;
            case 1:
                tempc = b_host[x] / b_host[x];
                break;
            case 2:
                b_host[x] = b_host[x] / 2;
            default:
                tempc = a[x] + b_host[x];
        }
        if (fabs(c[x] - tempc) > PRECISION){
            err = 1;
        }
    }

    return err;
}
#endif

int main(){
    int failcode = 0;
    int testrun;
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
