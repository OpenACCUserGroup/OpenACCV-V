#include "acc_testsuite.h"
#ifndef T1
//T1:parallel,loop,reduction,combined-constructs,V:1.0-2.7
int test1(){
    int err = 0;
    int m_n = 128;
    srand(SEED);
    real_t * a = (real_t *)malloc(10 * m_n * sizeof(real_t));
    real_t * b = (real_t *)malloc(10 * m_n * sizeof(real_t));
    real_t * c = (real_t *)malloc(10 * m_n * sizeof(real_t));
    real_t * totals = (real_t *)malloc(10 * sizeof(real_t));
    real_t temp;

    for (int x = 0; x < 10 * m_n; ++x){
        a[x] = .05 + rand() / (real_t)(RAND_MAX);
        b[x] = .05 + rand() / (real_t)(RAND_MAX);
        c[x] = 0.0;
    }


    #pragma acc data copyin(a[0:10*m_n], b[0:10*m_n]) copyout(c[0:10*m_n], totals[0:10])
    {
        #pragma acc parallel loop gang private(temp)
        for (int x = 0; x < 10; ++x){
            temp = 1;
            #pragma acc loop worker reduction(*:temp)
            for (int y = 0; y < m_n; ++y){
                temp *= a[x * m_n + y] + b[x * m_n + y];
            }
            totals[x] = temp;
            #pragma acc loop worker
            for (int y = 0; y < m_n; ++y){
                c[x * m_n + y] = (a[x * m_n + y] + b[x * m_n + y]) / totals[x];
            }
        }
    }

    for (int x = 0; x < 10; ++x){
        temp = 1;
        for (int y = 0; y < m_n; ++y){
            temp *= a[x * m_n + y] + b[x * m_n + y];
        }
        if (fabs(temp - totals[x]) > (temp / 2 + totals[x] / 2) * PRECISION){
            err += 1;
        }
        for (int y = 0; y < m_n; ++y){
            if (fabs(c[x * m_n + y] - ((a[x * m_n + y] + b[x * m_n + y]) / totals[x])) > PRECISION){
                err += 1;
            }
        }
    }

    return err;
}
#endif

#ifndef T2
//T2:parallel,private,reduction,combined-constructs,loop,V:2.7-2.7
int test2(){
    int err = 0;
    int m_n = 128;
    srand(SEED);
    real_t * a = (real_t *)malloc(25 * m_n * sizeof(real_t));
    real_t * b = (real_t *)malloc(25 * m_n * sizeof(real_t));
    real_t * c = (real_t *)malloc(25 * m_n * sizeof(real_t));

    real_t * totals = (real_t *)malloc(25 * sizeof(real_t));
    real_t reduced[5];

    for (int x = 0; x < n; ++x){
        a[x] = .05 + rand() / (real_t)(RAND_MAX / 10);
        b[x] = .05 + rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0;
    }

    #pragma acc data copyin(a[0:25*m_n], b[0:25*m_n]) copyout(c[0:25*m_n], totals[0:25])
    {
        #pragma acc parallel loop gang private(reduced)
        for (int x = 0; x < 5; ++x) {
            for (int y = 0; y < 5; ++y) {
                reduced[y] = 1.0;
            }
            #pragma acc loop worker reduction(*:reduced)
            for (int y = 0; y < 5 * m_n; ++y) {
                reduced[y%5] *= a[x * 5 * m_n + y] + b[x * 5 * m_n + y];
            }
            for (int y = 0; y < 5; ++y) {
                totals[x * 5 + y] = reduced[y];
            }
            for (int y = 0; y < 5 * m_n; ++y) {
                c[x * 5 * m_n + y] = (a[x * 5 * m_n + y] + b[x * 5 * m_n + y]) / totals[x * 5 + (y%5)];
            }
        }
    }

    for (int x = 0; x < 5; ++x) {
        for (int y = 0; y < 5; ++y) {
            reduced[y] = 1;
        }
        for (int y = 0; y < 5 * m_n; ++y) {
            reduced[y%5] *= a[x * 5 * m_n + y] + b[x * 5 * m_n + y];
        }
        for (int y = 0; y < 5; ++y) {
            if (fabs(totals[x * 5 + y] - reduced[y]) > PRECISION) {
                err += 1;
            }
        }
        for (int y = 0; y < 5 * m_n; ++y) {
            if (fabs(c[x * 5 * m_n + y] - ((a[x * 5 * m_n + y] + b[x * 5 * m_n + y]) / totals[x * 5 + (y % 5)])) > PRECISION) {
                err += 1;
            }
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
