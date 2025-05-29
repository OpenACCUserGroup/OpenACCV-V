#include "acc_testsuite.h"
bool is_possible(real_t* a, real_t* b, int length, real_t prev){
    if (length == 0){
        return true;
    }
    real_t *passed_a = (real_t *)malloc((length - 1) * sizeof(real_t));
    real_t *passed_b = (real_t *)malloc((length - 1) * sizeof(real_t));
    for (int x = 0; x < length; ++x){
        if (fabs(b[x] - prev) < PRECISION){
            for (int y = 0; y < x; ++y){
                passed_a[y] = a[y];
                passed_b[y] = b[y];
            }
            for (int y = x + 1; y < length; ++y){
                passed_a[y - 1] = a[y];
                passed_b[y - 1] = b[y];
            }
            if (is_possible(passed_a, passed_b, length - 1, a[x] + prev)){
                free(passed_a);
                free(passed_b);
                return true;
            }
        }
    }
    free(passed_a);
    free(passed_b);
    return false;
}

#ifndef T1
//T1:atomic,construct-independent,V:2.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t *a = (real_t *)malloc(n * sizeof(real_t));
    real_t *b = (real_t *)malloc(n * sizeof(real_t));
    real_t *c = (real_t *)malloc(n * sizeof(real_t));
    real_t *totals = (real_t *)malloc((n/10 + 1) * sizeof(real_t));
    real_t *totals_comparison = (real_t *)malloc((n/10 + 1) * sizeof(real_t));
    real_t *passed_ab = (real_t *)malloc(10 * sizeof(real_t));
    real_t *passed_c = (real_t *)malloc(10 * sizeof(real_t));
    int passed_indexer;
    int absolute_indexer;

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
    }
    for (int x = 0; x < n/10 + 1; ++x){
        totals[x] = 0;
        totals_comparison[x] = 0;
    }

    #pragma acc data copyin(a[0:n], b[0:n]) copy(totals[0:n/10 + 1]) copyout(c[0:n])
    {
        #pragma acc parallel
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                #pragma acc atomic capture
                {
                    c[x] = totals[x%(n/10 + 1)];
                    totals[x%(n/10 + 1)] = totals[x%(n/10 + 1)] + (a[x] * b[x]);
                }
            }
        }
    }

    for (int x = 0; x < n; ++x){
        totals_comparison[x%(n/10 + 1)] += a[x] * b[x];
    }
    for (int x = 0; x < n/10 + 1; ++x){
        if (fabs(totals_comparison[x] - totals[x]) > PRECISION){
            err += 1;
            break;
        }
    }

    for (int x = 0; x < n/10 + 1; ++x){
        for (passed_indexer = 0, absolute_indexer = x; absolute_indexer < n; passed_indexer++, absolute_indexer+= (n/10 + 1)){
            passed_ab[passed_indexer] = a[absolute_indexer] * b[absolute_indexer];
            passed_c[passed_indexer] = c[absolute_indexer];
        }
        if (!is_possible(passed_ab, passed_c, passed_indexer, 0)){
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
