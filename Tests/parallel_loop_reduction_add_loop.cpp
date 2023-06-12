#include "acc_testsuite.h"
#ifndef T1
//T1:parallel,loop,reduction,combined-constructs,V:1.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t * a = new real_t[10 * n];
    real_t* b = new real_t[10 * n];
    real_t* c = new real_t[10 * n];
    real_t* d = new real_t[10 * n];
    real_t avg = 0.0;
    real_t rolling_total = 0;

    for (int x = 0; x < 10 * n; ++x) {
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
    }

    #pragma acc data copyin(a[0:10*n], b[0:10*n]) create(c[0:10*n]) copyout(d[0:10*n])
    {
        #pragma acc parallel loop gang private(avg)
        for (int x = 0; x < 10; ++x) {
            avg = 0;
            #pragma acc loop worker reduction(+:avg)
            for (int y = 0; y < n; ++y) {
                avg += c[x * n + y] = a[x * n + y] + b[x * n + y];
            }
            avg = avg / n;
            #pragma acc loop worker
            for (int y = 0; y < n; ++y) {
                d[x * n + y] = c[x * n + y] - avg;
            }
        }
    }


    for (int x = 0; x < 10; ++x) {
        rolling_total = 0;
        for (int y = 0; y < n; ++y) {
            rolling_total += a[x * n + y] + b[x * n + y];
        }
        rolling_total = rolling_total / n;
        for (int y = 0; y < n; ++y) {
            if (fabs(d[x * n + y] - ((a[x * n + y] + b[x * n + y]) - rolling_total)) > PRECISION) {
                err += 1;
            }
        }
    }

    return err;
}
#endif

#ifndef T2
//T2:parallel,loop,reduction,combined-constructs,V:2.7-2.7
int test2() {
    int err = 0;
    srand(SEED);
    real_t* a = new real_t[25 * n];
    real_t* b = new real_t[25 * n];
    real_t* c = new real_t[25 * n];
    real_t* d = new real_t[25 * n];
    real_t avgs[5];
    real_t host_avgs[5];

    for (int x = 0; x < 25 * n; ++x) {
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0;
        d[x] = 0;
    }

    #pragma acc data copyin(a[0:25*n], b[0:25*n]) copyout(c[0:25*n], d[0:25*n])
    {
        #pragma acc parallel loop gang private(avgs)
        for (int x = 0; x < 5; ++x) {
            for (int y = 0; y < 5; ++y) {
                avgs[y] = 0;
            }
            #pragma acc loop worker reduction(+:avgs)
            for (int y = 0; y < 5 * n; ++y) {
                avgs[y % 5] += c[x * 5 * n + y] = a[x * 5 * n + y] + b[x * 5 * n + y];
            }
            for (int y = 0; y < 5; ++y) {
                avgs[y] = avgs[y] / n;
            }
            #pragma acc loop worker
            for (int y = 0; y < 5 * n; ++y) {
                d[x * 5 * n + y] = c[x * 5 * n + y] - avgs[y % 5];
            }
        }
    }

    for (int x = 0; x < 5; ++x) {
        for (int y = 0; y < 5; ++y) {
            host_avgs[y] = 0;
        }
        for (int y = 0; y < 5 * n; ++y) {
            host_avgs[y % 5] += a[x * 5 * n + y] + b[x * 5 * n + y];
            if (fabs(c[x * 5 * n + y] - (a[x * n * 5 + y] + b[x * n * 5 + y])) > PRECISION) {
                err += 1;
            }
        }
        for (int y = 0; y < 5; ++y) {
            host_avgs[y] = host_avgs[y] / n;
        }
        for (int y = 0; y < 5 * n; ++y) {
            if (fabs(d[x * 5 * n + y] - (c[x * 5 * n + y] - host_avgs[y % 5])) > PRECISION){
                err += 1;
            
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
    return failcode;
}
