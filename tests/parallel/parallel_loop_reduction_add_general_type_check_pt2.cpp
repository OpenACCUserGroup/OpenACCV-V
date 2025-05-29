#include "acc_testsuite.h"
#ifndef T1
//T1:parallel,reduction,combined-constructs,loop,V:1.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    unsigned short int * a = (unsigned short int*)malloc(n * sizeof(unsigned short int));
    unsigned short int * b = (unsigned short int *)malloc(n * sizeof(unsigned short int));
    unsigned short int total = 10;
    unsigned short int host_total = 10;

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
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

    if (host_total != total) {
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
    unsigned int * a = (unsigned int *)malloc(n * sizeof(unsigned int));
    unsigned int * b = (unsigned int *)malloc(n * sizeof(unsigned int));
    unsigned int total = 10;
    unsigned int host_total = 10;

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
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

    if (host_total != total) {
        err += 1;
    }

    return err;
}
#endif

#ifndef T3
//T3:parallel,reduction,combined-constructs,loop,V:1.0-2.7
int test3(){
    int err = 0;
    srand(SEED);
    unsigned long int * a = (unsigned long int *)malloc(n * sizeof(unsigned long int));
    unsigned long int * b = (unsigned long int *)malloc(n * sizeof(unsigned long int));
    unsigned long int total = 10;
    unsigned long int host_total = 10;

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
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

    if (host_total != total) {
        err += 1;
    }

    return err;
}
#endif

#ifndef T4
//T4:parallel,reduction,combined-constructs,loop,V:1.0-2.7
int test4(){
    int err = 0;
    srand(SEED);
    unsigned long long int * a = (unsigned long long int *)malloc(n * sizeof(unsigned long long int));
    unsigned long long int * b = (unsigned long long int *)malloc(n * sizeof(unsigned long long int));
    unsigned long long int total = 10;
    unsigned long long int host_total = 10;

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
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
    
    if (total != host_total) {
        err += 1;
    }

    return err;
}
#endif

#ifndef T5
//T5:parallel,reduction,combined-constructs,loop,V:1.0-2.7
int test5(){
    int err = 0;
    srand(SEED);
    float * a = new float[n];
    float * b = new float[n];
    float total = 10;
    float host_total = 10;

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
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

    if (fabsf(total - host_total) > PRECISION) {
        err += 1;
    }

    return err;
}
#endif

#ifndef T6
//T6:parallel,reduction,combined-constructs,loop,V:1.0-2.7
int test6(){
    int err = 0;
    srand(SEED);
    double * a = new double[n];
    double * b = new double[n];
    double total = 10;
    double host_total = 10;

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
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

    if (fabs(host_total - total) > PRECISION) {
        err += 1;
    }

    return err;
}
#endif

#ifndef T7
//T7:parallel,reduction,combined-constructs,loop,V:1.0-2.7
int test7(){
    int err = 0;
    srand(SEED);
    long double * a = (long double *)malloc(n * sizeof(long double));
    long double * b = (long double *)malloc(n * sizeof(long double));
    long double total = 10;
    long double host_total = 10;

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
    }
    
    #pragma acc data copyin(a[0:n], b[0:n])
    {
        #pragma acc parallel loop reduction(+:total)
        for (int x = 0; x < n; ++x){
            total += a[x] + b[x];
        }
    }
    
    for (int x = 0; x < n; ++x) {
        host_total += a[x] + b[x];
    }

    if (fabsl(host_total - total) > PRECISION) {
        err += 1;
    }

    return err;
}
#endif

#ifndef T8
//T8:parallel,reduction,combined-constructs,loop,V:1.0-2.7
int test8(){
    int err = 0;
    srand(SEED);
    float _Complex * a = (float _Complex *)malloc(n * sizeof(float _Complex));
    float _Complex * b = (float _Complex *)malloc(n * sizeof(float _Complex));
    float _Complex total = 10 + 10 * I;
    float _Complex host_total = 10 + 10 * I;

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

    if (fabsf(crealf(total) - crealf(host_total)) > PRECISION) {
        err += 1;
    }
    if (fabsf(cimagf(total) - cimagf(host_total)) > PRECISION) {
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
#ifndef T3
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x){
        failed = failed + test3();
    }
    if (failed != 0){
        failcode = failcode + (1 << 2);
    }
#endif
#ifndef T4
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x){
        failed = failed + test4();
    }
    if (failed != 0){
        failcode = failcode + (1 << 3);
    }
#endif
#ifndef T5
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x){
        failed = failed + test5();
    }
    if (failed != 0){
        failcode = failcode + (1 << 4);
    }
#endif
#ifndef T6
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x){
        failed = failed + test6();
    }
    if (failed != 0){
        failcode = failcode + (1 << 5);
    }
#endif
#ifndef T7
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x){
        failed = failed + test7();
    }
    if (failed != 0){
        failcode = failcode + (1 << 6);
    }
#endif
#ifndef T8
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x){
        failed = failed + test8();
    }
    if (failed != 0){
        failcode = failcode + (1 << 7);
    }
#endif
    return failcode;
}

