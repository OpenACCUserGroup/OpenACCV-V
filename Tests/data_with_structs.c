#include "acc_testsuite.h"
typedef struct multi_item {
    real_t a;
    real_t b;
    real_t c;
} multi_item;

#ifndef T1
//T1:data,data-region,construct-independent,V:1.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    multi_item * a = (multi_item *)malloc(n * sizeof(multi_item));

    for (int x = 0; x < n; ++x){
        a[x].a = rand() / (real_t)(RAND_MAX / 10);
        a[x].b = rand() / (real_t)(RAND_MAX / 10);
        a[x].c = rand() / (real_t)(RAND_MAX / 10);
    }

    #pragma acc data copy(a[0:n])
    {
        #pragma acc parallel
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                a[x].c = a[x].a + a[x].b;
            }
        }
    }

    for (int x = 0; x < n; ++x){
        if (fabs(a[x].c - (a[x].a + a[x].b)) > PRECISION){
            err += 1;
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
