#include "acc_testsuite.h"
#ifndef T1
//T1:parallel,loop,reduction,combined-constructs,V:1.0-2.7
int test1(){
    int err = 0;
    int multiplicitive_n = 128;
    srand(SEED);
    real_t * a = (real_t *)malloc(multiplicitive_n * sizeof(real_t));
    real_t * b = (real_t *)malloc(multiplicitive_n * sizeof(real_t));

    real_t multiplied_total = 1.0;

    for (int x = 0; x < multiplicitive_n; ++x){
        a[x] = rand() / (real_t) RAND_MAX;
        b[x] = rand() / (real_t) RAND_MAX;
    }

    #pragma acc data copyin(a[0:multiplicitive_n], b[0:multiplicitive_n]) copy(multiplied_total)
    {
        #pragma acc parallel loop reduction(*:multiplied_total)
        for (int x = 0; x < multiplicitive_n; ++x){
            multiplied_total *= a[x] + b[x];
        }
    }

    for (int x = 0; x < multiplicitive_n; ++x){
        multiplied_total /= (a[x] + b[x]);
    }
    if (fabs(multiplied_total - 1) > PRECISION * (2 * multiplicitive_n - 1)){
        err = 1;
    }

    return err;
}
#endif

#ifndef T2
//T2:parallel,reduction,combined-constructs,loop,V:2.7-2.7
int test2(){
    int err = 0;
    int m_n = 128;
    srand(SEED);
    real_t * a = (real_t *)malloc(10 * m_n * sizeof(real_t));
    real_t * b = (real_t *)malloc(10 * m_n * sizeof(real_t));

    real_t multiplicitive_total[10];

    for (int x = 0; x < m_n; ++x){
        a[x] = rand() / (real_t) RAND_MAX;
        b[x] = rand() / (real_t) RAND_MAX;
    }

    for (int x = 0; x < 10; ++x) {
        multiplicitive_total[x] = 1.0;
    }

    #pragma acc data copyin(a[0:10*m_n], b[0:10*m_n])
    {
        #pragma acc parallel loop reduction(*:multiplicitive_total)
        for (int x = 0; x < 10 * m_n; ++x) {
            multiplicitive_total[x%10] = multiplicitive_total[x%10] * (a[x] + b[x]);
        }
    }

    for (int x = 0; x < 10 * m_n; ++x) {
        multiplicitive_total[x%10] /= (a[x] + b[x]);
    }

    for (int x = 0; x < 10; ++x) {
        if (fabs(multiplicitive_total[x]) > PRECISION) {
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
