#include "acc_testsuite.h"
void test_routine_vector_loop_named(real_t ** a, real_t * b, long long n);
void test_routine_vector_vector_named(real_t ** a, real_t * b, long long n);
void test_routine_vector_seq_named(real_t ** a, real_t * b, long long n);

#pragma acc routine(test_routine_vector_loop_named) vector
#pragma acc routine(test_routine_vector_vector_named) vector
#pragma acc routine(test_routine_vector_seq_named) vector

#pragma acc routine vector
real_t called_function_vector(real_t **a, int x, long long n){
    real_t returned = 0;
    #pragma acc loop vector reduction(+:returned)
    for (int y = 0; y < n; ++y){
        returned += a[x][y];
    }
    return returned;
}

#pragma acc routine seq
real_t called_function_seq(real_t **a, int x, long long n){
    real_t returned = 0;
    #pragma acc loop seq reduction(+:returned)
    for (int y = 0; y < n; ++y){
        returned += a[x][y];
    }
    return returned;
}

#pragma acc routine vector
void test_routine_vector_loop_unnamed(real_t ** a, real_t * b, long long n){
    #pragma acc loop vector
    for (int x = 0; x < n; ++x){
        for (int y = 0; y < n; ++y){
            b[x] += a[x][y];
        }
    }
}

#pragma acc routine vector
void test_routine_vector_vector_unnamed(real_t ** a, real_t * b, long long n){
    for (int x = 0; x < n; ++x){
        b[x] = called_function_vector(a, x, n);
    }
}

#pragma acc routine vector
void test_routine_vector_seq_unnamed(real_t ** a, real_t *b, long long n){
    for (int x = 0; x < n; ++x){
        b[x] = called_function_seq(a, x, n);
    }
}

void test_routine_vector_loop_named(real_t ** a, real_t * b, long long n){
    #pragma acc loop vector
    for (int x = 0; x < n; ++x){
        for (int y = 0; y < n; ++y){
            b[x] += a[x][y];
        }
    }
}

void test_routine_vector_vector_named(real_t ** a, real_t * b, long long n){
    for (int x = 0; x < n; ++x){
        b[x] = called_function_vector(a, x, n);
    }
}

void test_routine_vector_seq_named(real_t ** a, real_t * b, long long n){
    for (int x = 0; x < n; ++x){
        b[x] = called_function_seq(a, x, n);
    }
}

bool validate_data(real_t ** a, real_t * b){
    real_t local_total;
    for (int x = 0; x < n; ++x){
        local_total = 0;
        for (int y = 0; y < n; ++y){
            local_total += a[x][y];
        }
        if (fabs(b[x] - local_total) > PRECISION){
            return true;
        }
    }
    return false;
}

#ifndef T1
//T1:routine,construct-independent,V:2.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t ** a = (real_t **)malloc(n * sizeof(real_t *));
    real_t * b = new real_t[n];

    for (int x = 0; x < n; ++x){
        a[x] = new real_t[n];
    }

    for (int x = 0; x < n; ++x){
        for (int y = 0; y < n; ++y){
            a[x][y] = rand() / (real_t)(RAND_MAX / 10);
        }
        b[x] = 0;
    }

    #pragma acc data copyin(a[0:n][0:n]) copy(b[0:n])
    {
        #pragma acc parallel
        {
              test_routine_vector_loop_unnamed(a, b, n);
        }
    }

    if (validate_data(a, b)){
        err += 1;
    }

    return err;
}
#endif

#ifndef T2
//T2:routine,construct-independent,V:2.0-2.7
int test2(){
    int err = 0;
    srand(SEED);
    real_t ** a = (real_t **)malloc(n * sizeof(real_t *));
    real_t * b = new real_t[n];

    for (int x = 0; x < n; ++x){
        a[x] = new real_t[n];
    }

    for (int x = 0; x < n; ++x){
        for (int y = 0; y < n; ++y){
            a[x][y] = rand() / (real_t)(RAND_MAX / 10);
        }
        b[x] = 0;
    }

    #pragma acc data copyin(a[0:n][0:n]) copy(b[0:n])
    {
        #pragma acc parallel
        {
            test_routine_vector_loop_named(a, b, n);
        }
    }

    if (validate_data(a, b)){
        err += 1;
    }

    return err;
}
#endif

#ifndef T3
//T3:routine,construct-independent,V:2.0-2.7
int test3(){
    int err = 0;
    srand(SEED);
    real_t ** a = (real_t **)malloc(n * sizeof(real_t *));
    real_t * b = new real_t[n];

    for (int x = 0; x < n; ++x){
        a[x] = new real_t[n];
    }

    for (int x = 0; x < n; ++x){
        for (int y = 0; y < n; ++y){
            a[x][y] = rand() / (real_t)(RAND_MAX / 10);
        }
        b[x] = 0;
    }

    #pragma acc data copyin(a[0:n][0:n]) copy(b[0:n])
    {
        #pragma acc parallel
        {
            test_routine_vector_vector_unnamed(a, b, n);
        }
    }

    if (validate_data(a, b)){
        err += 1;
    }

    return err;
}
#endif

#ifndef T4
//T4:routine,construct-independent,V:2.0-2.7
int test4(){
    int err = 0;
    srand(SEED);
    real_t ** a = (real_t **)malloc(n * sizeof(real_t *));
    real_t * b = new real_t[n];

    for (int x = 0; x < n; ++x){
        a[x] = new real_t[n];
    }

    for (int x = 0; x < n; ++x){
        for (int y = 0; y < n; ++y){
            a[x][y] = rand() / (real_t)(RAND_MAX / 10);
        }
        b[x] = 0;
    }

    #pragma acc data copyin(a[0:n][0:n]) copy(b[0:n])
    {
        #pragma acc parallel
        {
            test_routine_vector_vector_named(a, b, n);
        }
    }

    if (validate_data(a, b)){
        err += 1;
    }

    return err;
}
#endif

#ifndef T5
//T5:routine,construct-independent,V:2.0-2.7
int test5(){
    int err = 0;
    srand(SEED);
    real_t ** a = (real_t **)malloc(n * sizeof(real_t *));
    real_t * b = new real_t[n];

    for (int x = 0; x < n; ++x){
        a[x] = new real_t[n];
    }

    for (int x = 0; x < n; ++x){
        for (int y = 0; y < n; ++y){
            a[x][y] = rand() / (real_t)(RAND_MAX / 10);
        }
        b[x] = 0;
    }

    #pragma acc data copyin(a[0:n][0:n]) copy(b[0:n])
    {
        #pragma acc parallel
        {
            test_routine_vector_seq_unnamed(a, b, n);
        }
    }

    if (validate_data(a, b)){
        err += 1;
    }

    return err;
}
#endif

#ifndef T6
//T6:routine,construct-independent,V:2.0-2.7
int test6(){
    int err = 0;
    srand(SEED);
    real_t ** a = (real_t **)malloc(n * sizeof(real_t *));
    real_t * b = new real_t[n];

    for (int x = 0; x < n; ++x){
        a[x] = new real_t[n];
    }

    for (int x = 0; x < n; ++x){
        for (int y = 0; y < n; ++y){
            a[x][y] = rand() / (real_t)(RAND_MAX / 10);
        }
        b[x] = 0;
    }

    #pragma acc data copyin(a[0:n][0:n]) copy(b[0:n])
    {
        #pragma acc parallel
        {
            test_routine_vector_seq_named(a, b, n);
        }
    }

    if (validate_data(a, b)){
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
    return failcode;
}
