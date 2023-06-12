#include "acc_testsuite.h"
#ifndef T1
//T1:kernels,loop,reduction,combined-constructs,V:1.0-2.7
int test1(){
    int err = 0;
    int multiplicitive_n = 128;
    srand(SEED);
    real_t * a = new real_t[10 * multiplicitive_n];
    real_t * b = new real_t[10 * multiplicitive_n];
    real_t * c = new real_t[10];
    real_t temp;

    for (int x = 0; x < 10 * multiplicitive_n; ++x){
        a[x] = rand() / (real_t) RAND_MAX;
        b[x] = rand() / (real_t) RAND_MAX;
    }

    #pragma acc data copyin(a[0:10*multiplicitive_n], b[0:10*multiplicitive_n]) copyout(c[0:10])
    {
        #pragma acc kernels loop private(temp)
        for (int x = 0; x < 10; ++x){
            temp = 1.0;
            #pragma acc loop vector reduction(*:temp)
            for (int y = 0; y < multiplicitive_n; ++y){
                temp *= a[(x * multiplicitive_n) + y] + b[(x * multiplicitive_n) + y];
            }
            c[x] = temp;
        }
    }

    for (int x = 0; x < 10; ++x){
        for (int y = 0; y < multiplicitive_n; ++y){
            c[x] /= a[(x * multiplicitive_n) + y] + b[(x * multiplicitive_n) + y];
        }
    }

    for (int x = 0; x < 10; ++x){
        if (fabs(c[x] - 1) > PRECISION * (4 * multiplicitive_n - 1)){
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
