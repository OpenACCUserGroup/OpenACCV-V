#include "acc_testsuite.h"
#ifndef T1
//T1:parallel,reduction,combined-constructs,loop,V:1.0-2.7
int test1() {
    int err = 0;
    srand(SEED);
    bool* a = new bool[n];
    bool* b = new bool[n];
    bool total = 1;
    bool host_total = 1;

    for (int x = 0; x < n; ++x) {
        if ((rand()/((real_t) RAND_MAX)) > .5) {
            a[x] = 1;
        }
        else {
            a[x] = 0;
        }
        if ((rand()/((real_t) RAND_MAX)) > .5) {
            b[x] = 1;
        }
        else {
            b[x] = 0;
        }
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

#ifndef T2
//T2:parallel,reduction,combined-constructs,loop,V:1.0-2.7
int test2(){
    int err = 0;
    srand(SEED);
    char * a = new char[n];
    char * b = new char[n];
    char total = 10;
    char host_total = 10;

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(UCHAR_MAX / 10);
        b[x] = rand() / (real_t)(UCHAR_MAX / 10);
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
    signed char * a = (signed char *)malloc(n * sizeof(signed char));
    signed char * b = (signed char *)malloc(n * sizeof(signed char));
    signed char total = 10;
    signed char host_total = 10;

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
    unsigned char * a = (unsigned char *)malloc(n * sizeof(unsigned char));
    unsigned char * b = (unsigned char *)malloc(n * sizeof(unsigned char));
    unsigned char total = 10;
    unsigned char host_total = 10;

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
    short int * a = (short int *)malloc(n * sizeof(short int));
    short int * b = (short int *)malloc(n * sizeof(short int));
    short int total = 10;
    short int host_total = 10;    

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

#ifndef T6
//T6:parallel,reduction,combined-constructs,loop,V:1.0-2.7
int test6(){
    int err = 0;
    srand(SEED);
    int * a = new int[n];
    int * b = new int[n];
    int total = 10;
    int host_total = 10;

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

#ifndef T7
//T7:parallel,reduction,combined-constructs,loop,nonvalidating,V:1.0-2.7
int test7(){
    int err = 0;
    srand(SEED);
    long int * a = (long int *)malloc(n * sizeof(long int));
    long int * b = (long int *)malloc(n * sizeof(long int));
    long int total = 10;
    long int host_total = 10;

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

    return err;
}
#endif

#ifndef T8
//T8:parallel,reduction,combined-constructs,loop,V:1.0-2.7
int test8(){
    int err = 0;
    srand(SEED);
    long long int * a = (long long int *)malloc(n * sizeof(long long int));
    long long int * b = (long long int *)malloc(n * sizeof(long long int));
    long long int total = 10;
    long long int host_total = 10;

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

int main() {
    int failcode = 0;
    int failed;
#ifndef T1
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x) {
        failed = failed + test1();
    }
    if (failed != 0) {
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
