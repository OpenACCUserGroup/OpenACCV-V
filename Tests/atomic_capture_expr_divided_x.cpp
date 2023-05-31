#include "acc_testsuite.h"
bool is_possible(real_t* a, real_t* b, int length, real_t prev){
    if (length == 0){
        return true;
    }
    real_t *passed_a = new real_t[(length - 1)];
    real_t *passed_b = new real_t[(length - 1)];
    for (int x = 0; x < length; ++x){
        if (fabs(b[x] - (a[x]/prev)) < PRECISION){
            for (int y = 0; y < x; ++y){
                passed_a[y] = a[y];
                passed_b[y] = b[y];
            }
            for (int y = x + 1; y < length; ++y){
                passed_a[y - 1] = a[y];
                passed_b[y - 1] = b[y];
            }
            if (is_possible(passed_a, passed_b, length - 1, b[x])){
                delete[] passed_a;
                delete[] passed_b;
                return true;
            }
        }
    }
    delete[] passed_a;
    delete[] passed_b;
    return false;
}

bool possible_result(real_t * remaining_combinations, int length, real_t current_value, real_t test_value){
    if (length == 0){
        if (fabs(current_value - test_value) > PRECISION){
            return true;
        }
        else {
            return false;
        }
    }
    real_t * passed = new real_t[(length - 1)];
    for (int x = 0; x < length; ++x){
        for (int y = 0; y < x; ++y){
            passed[y] = remaining_combinations[y];
        }
        for (int y = x + 1; y < length; ++y){
            passed[y - 1] = remaining_combinations[y];
        }
        if (possible_result(passed, length - 1, remaining_combinations[x] / current_value, test_value)){
            delete[] passed;
            return true;
        }
    }
    delete[] passed;
    return false;
}

#ifndef T1
//T1:atomic,construct-independent,V:2.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t *a = new real_t[n];
    real_t *b = new real_t[n];
    real_t *c = new real_t[n];
    real_t *totals = new real_t[(n/10 + 1)];
    real_t *passed_ab = new real_t[10];
    real_t *passed_c = new real_t[10];
    int passed_indexer;
    int absolute_indexer;
    real_t * passed = new real_t[10];
    int indexer;

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
    }
    for (int x = 0; x < n/10 + 1; ++x){
        totals[x] = 1;
    }

    #pragma acc data copyin(a[0:n], b[0:n]) copy(totals[0:n/10 + 1]) copyout(c[0:n])
    {
        #pragma acc parallel
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                #pragma acc atomic capture
                    c[x] = totals[x%(n/10 + 1)] = (a[x] + b[x]) / totals[x%(n/10 + 1)];
            }
        }
    }


    for (int x = 0; x < (n/10 + 1); ++x){
        indexer = x;
        while (indexer < n){
            passed[indexer/(n/10 + 1)] = (a[x] + b[x]);
            indexer += (n/10 + 1);
        }
        if (!(possible_result(passed, 10, 1, totals[x]))){
            err += 1;
        }
    }

    for (int x = 0; x < (n/10 + 1); ++x){
        for (passed_indexer = 0, absolute_indexer = x; absolute_indexer < n; passed_indexer++, absolute_indexer += n/10 + 1){
            passed_ab[passed_indexer] = a[absolute_indexer] + b[absolute_indexer];
            passed_c[passed_indexer] = c[absolute_indexer];
        }
        if (!is_possible(passed_ab, passed_c, passed_indexer, 1)){
            err++;
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
