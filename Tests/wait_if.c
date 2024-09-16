#include "acc_testsuite.h"

#ifndef T1
//T1:parallel,wait,async,if,V:2.7-3.3
int test1(){
    int err = 0;
    srand(SEED);
    real_t *a = (real_t *)malloc(n * sizeof(real_t));
    real_t *b = (real_t *)malloc(n * sizeof(real_t));
    real_t *c = (real_t *)malloc(n * sizeof(real_t));
    real_t *d = (real_t *)malloc(n * sizeof(real_t));
    real_t *e = (real_t *)malloc(n * sizeof(real_t));
    real_t *f = (real_t *)malloc(n * sizeof(real_t));

    for (int x = 0; x < n; ++x){
	a[x] = rand() / (real_t)(RAND_MAX / 10);
	b[x] = rand() / (real_t)(RAND_MAX / 10);
	c[x] = 0.0;
	d[x] = rand() / (real_t)(RAND_MAX / 10);
	e[x] = rand() / (real_t)(RAND_MAX / 10);
	f[x] = 0.0;
    }

    #pragma acc data copyin(a[0:n], b[0:n], d[0:n], e[0:n]) create(c[0:n], f[0:n])
    {
	#pragma acc parallel async(1)
	{
	    #pragma acc loop
	    for (int x = 0; x < n; ++x){
		c[x] = a[x] + b[x];
	    }
	}
	#pragma acc parallel async(2)
	{
	    #pragma acc loop
	    for (int x = 0; x < n; ++x){
		f[x] = d[x] + e[x];
	    }
	}
	#pragma acc update host(c[0:n], f[0:n]) wait(1, 2) if(true)
    }
    
    for (int x = 0; x < n; ++x){
	if (abs(c[x] - (a[x] + b[x])) > PRECISION){
	    err++;
	}
	if (abs(f[x] - (d[x] + e[x])) > PRECISION){
	    err++;
	}
    }

    return err;
}
#endif
#ifndef T2 
//T2:parallel,wait,async,if,V:2.7-3.3
int test2(){
    int err = 0;
    srand(SEED);
    real_t *a = (real_t *)malloc(n * sizeof(real_t));
    real_t *b = (real_t *)malloc(n * sizeof(real_t));
    real_t *c = (real_t *)malloc(n * sizeof(real_t));
    real_t *d = (real_t *)malloc(n * sizeof(real_t));
    real_t *e = (real_t *)malloc(n * sizeof(real_t));
    real_t *f = (real_t *)malloc(n * sizeof(real_t));

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0.0;
        d[x] = rand() / (real_t)(RAND_MAX / 10);
        e[x] = rand() / (real_t)(RAND_MAX / 10);
        f[x] = 0.0;
    }

    #pragma acc data copyin(a[0:n], b[0:n], d[0:n], e[0:n]) create(c[0:n], f[0:n])
    {
        #pragma acc parallel async(1)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                c[x] = a[x] + b[x];
            }
        }
        #pragma acc parallel async(2)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                f[x] = d[x] + e[x];
            }
        }
        #pragma acc update host(c[0:n], f[0:n]) wait(1) if(true)
	#pragma acc update host(c[0:n], f[0:n]) wait(2) if(true)
    }

    for (int x = 0; x < n; ++x){
        if (abs(c[x] - (a[x] + b[x])) > PRECISION){
            err++;
        }
        if (abs(f[x] - (d[x] + e[x])) > PRECISION){
            err++;
        }
    }

    return err;
}
#endif
#ifndef T3
//T3:parallel,wait,async,if,V:2.7-3.3
int test3(){
    int err = 0;
    srand(time(NULL));
    real_t *a = (real_t *)malloc(n * sizeof(real_t));
    real_t *b = (real_t *)malloc(n * sizeof(real_t));
    real_t *c = (real_t *)malloc(n * sizeof(real_t));
    real_t *d = (real_t *)malloc(n * sizeof(real_t));
    real_t *e = (real_t *)malloc(n * sizeof(real_t));
    real_t *f = (real_t *)malloc(n * sizeof(real_t));
    
    for (int x = 0; x < n; ++x){
	a[x] = rand() / (real_t)(RAND_MAX / 10);
	b[x] = rand() / (real_t)(RAND_MAX / 10);
	c[x] = 0.0;
	d[x] = rand() / (real_t)(RAND_MAX / 10);
	e[x] = rand() / (real_t)(RAND_MAX / 10);
	f[x] = 0.0;
    }

    #pragma acc data copyin(a[0:n], b[0:n], d[0:n], e[0:n]) create(c[0:n], f[0:n])
    {
	#pragma acc parallel async(1)
	{
	    #pragma acc loop
	    for (int x = 0; x < n; ++x){
	        c[x] = a[x] + b[x];
	    }
 	}
	#pragma acc parallel async(2)
	{
	    #pragma acc loop
	    for (int x = 0; x < n; ++x){
		f[x] = d[x] + e[x];
	    }
	}
	#pragma acc update host(c[0:n], f[0:n]) wait(1, 2) if(false)
    }
        
    for (int x = 0; x < n; ++x){
	if (c[x] > PRECISION){
	    err++;
	}
	if (f[x] > PRECISION){
	    err++;
	}
    }
    return err;
}
#endif
#ifndef T4
//T4:parallel,wait,async,if,V:2.7-3.3
int test4(){
    int err = 0;
    srand(time(NULL));
    real_t *a = (real_t *)malloc(n * sizeof(real_t));
    real_t *b = (real_t *)malloc(n * sizeof(real_t));
    real_t *c = (real_t *)malloc(n * sizeof(real_t));
    real_t *d = (real_t *)malloc(n * sizeof(real_t));
    real_t *e = (real_t *)malloc(n * sizeof(real_t));
    real_t *f = (real_t *)malloc(n * sizeof(real_t));

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0.0;
        d[x] = rand() / (real_t)(RAND_MAX / 10);
        e[x] = rand() / (real_t)(RAND_MAX / 10);
        f[x] = 0.0;
    }

    #pragma acc data copyin(a[0:n], b[0:n], d[0:n], e[0:n]) create(c[0:n], f[0:n])
    {
        #pragma acc parallel async(1)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                c[x] = a[x] + b[x];
            }
        }
        #pragma acc parallel async(2)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                f[x] = d[x] + e[x];
            }
        }
        #pragma acc update host(c[0:n], f[0:n]) wait(1) if(false)
	#pragma acc update host(c[0:n], f[0:n]) wait(2) if(false)
    }

    for (int x = 0; x < n; ++x){
        if (c[x] > PRECISION){
            err++;
        }
        if (f[x] > PRECISION){
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
        failed += test1();
    }
    if (failed){
        failcode += (1 << 0);
    }
#endif
#ifndef T2
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x){
	failed += test2();
    }
    if (failed){
	failcode += (1 << 1);
    }
#endif
#ifndef T3
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x){
        failed += test3();
    }
    if (failed){
        failcode += (1 << 2);
    }
#endif
#ifndef T4
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x){
        failed += test4();
    }
    if (failed){
        failcode += (1 << 3);
    }
#endif
    return failcode;
}
