#include "acc_testsuite.h"
#ifndef T1
//T1:parallel,reduction,combined-constructs,loop,V:1.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    double _Complex * a = (double _Complex *)malloc(n * sizeof(double _Complex));
    double _Complex * b = (double _Complex *)malloc(n * sizeof(double _Complex));
    double _Complex total = 10 + 10 * I;
    double _Complex host_total = 10 + 10 * I;

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10) + rand() / (real_t)(RAND_MAX / 10) * I;
        b[x] = rand() / (real_t)(RAND_MAX / 10) + rand() / (real_t)(RAND_MAX / 10) * I;
    }

    #pragma acc data copyin(a[0:n], b[0:n])
    {
        #pragma acc parallel loop reduction(+:total)
        for (int x = 0; x < n; ++x) {
            total += a[x] + b[x];
        }
    }

    for (int x = 0; x < n; ++x) {
        host_total += a[x] + b[x];
    }

    if (fabs(creal(total) - creal(host_total)) > PRECISION) {
        err += 1;
    }
    if (fabs(cimag(total) - cimag(host_total)) > PRECISION) {
        err += 1;
    }

    return err;
}
#endif


#ifndef T2
//T2:parallel,reduction,combined-constructs,loop,V:1.0-2.7
int test2(){
    int err = 0;
    srand(SEED);
    long double _Complex * a = (long double _Complex*)malloc(n * sizeof(long double _Complex));
    long double _Complex * b = (long double _Complex*)malloc(n * sizeof(long double _Complex));
    long double _Complex total = 10 + 10 * I;
    long double _Complex host_total = 10 + 10 * I;

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10) + rand() / (real_t)(RAND_MAX / 10) * I;
        b[x] = rand() / (real_t)(RAND_MAX / 10) + rand() / (real_t)(RAND_MAX / 10) * I;
    }

    #pragma acc data copyin(a[0:n], b[0:n])
    {
        #pragma acc parallel loop reduction(+:total)
        for (int x = 0; x < n; ++x) {
            total += a[x] + b[x];
        }
    }

    for (int x = 0; x < n; ++x) {
        host_total += a[x] + b[x];
    }

    if (fabsl(creall(total) - creall(host_total)) > PRECISION) {
        err += 1;
    }
    if (fabsl(cimagl(total) - cimagl(host_total)) > PRECISION) {
        err += 1;
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

