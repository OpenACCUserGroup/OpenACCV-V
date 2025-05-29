#include "acc_testsuite.h"

using namespace std;

#ifndef T1
//T1:parallel,wait,async,V:2.7-3.2
int test1(){
    int err = 0;
    srand(time(NULL));
    data_container<real_t> a = *(new data_container<real_t>(n));
    data_container<real_t> b = *(new data_container<real_t>(n));
    data_container<real_t> c = *(new data_container<real_t>(n));
    data_container<real_t> d = *(new data_container<real_t>(n));
    data_container<real_t> e = *(new data_container<real_t>(n));
    data_container<real_t> f = *(new data_container<real_t>(n));
    data_container<real_t> g = *(new data_container<real_t>(n));

    for (int x = 0; x < n; ++x){
        a.data[x] = rand() / (real_t)(RAND_MAX / 10);
        b.data[x] = rand() / (real_t)(RAND_MAX / 10);
	c.data[x] = 0.0;
        d.data[x] = rand() / (real_t)(RAND_MAX / 10);
        e.data[x] = rand() / (real_t)(RAND_MAX / 10);
	f.data[x] = 0.0;
	g.data[x] = 0.0;
    }

    #pragma acc data copyin(a,a.data[0:n], b, b.data[0:n], c, c.data[0:n], d, d.data[0:n], e, e.data[0:n], f, f.data[0:n],g, g.data[0:n])
    {
        #pragma acc parallel loop async(1)
        for (int x = 0; x < n; ++x){
            c.data[x] = a.data[x] + b.data[x];
        }
        #pragma acc parallel loop async(2)
        for (int x = 0; x < n; ++x){
            f.data[x] = d.data[x] + e.data[x];
        }
        #pragma acc parallel wait(1, 2) async(3)
        for (int x = 0; x < n; ++x){
            g.data[x] = c.data[x] + f.data[x];
        }
    
    	#pragma acc update host(c.data[0:n]) wait(1) if(true)
	#pragma acc update host(f.data[0:n]) wait(2) if(true)
	#pragma acc update host(g.data[0:n]) wait(3) if(true)
    }

    for (int x = 0; x < n; ++x){
        if (abs(c.data[x] - (a.data[x] + b.data[x])) > PRECISION){
            err++;
        }
        if (abs(f.data[x] - (d.data[x] + e.data[x])) > PRECISION){
	    err++;
	}
        if (abs(g.data[x] - (c.data[x] + f.data[x])) > PRECISION){
            err++;
      	}
    }

    return err;
}
#endif
#ifndef T2
//T2:parallel,wait,async,V:2.7-3.2
int test2(){
    int err = 0;
    srand(time(NULL));

    real_t * a = new real_t[n];
    real_t * b = new real_t[n];
    real_t * c = new real_t[n];
    real_t * d = new real_t[n];
    real_t * e = new real_t[n];
    real_t * f = new real_t[n];
    real_t * g = new real_t[n];

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0.0;
        d[x] = rand() / (real_t)(RAND_MAX / 10);
        e[x] = rand() / (real_t)(RAND_MAX / 10);
        f[x] = 0.0;
        g[x] = 0.0;
    }

   #pragma acc data copyin(a[0:n], b[0:n], c[0:n], d[0:n], e[0:n], f[0:n], g[0:n])
    {
        #pragma acc parallel loop async(1)
        for (int x = 0; x < n; ++x){
            c[x] = (a[x] + b[x]);
        }
        #pragma acc parallel loop async(2)
        for (int x = 0; x < n; ++x){
            f[x] = (d[x] + e[x]);
        }
        #pragma acc parallel loop wait(1, 2) async(3)
        for (int x = 0; x < n; ++x){
            g[x] = (c[x] + f[x]);
        }
        #pragma acc update host(c[0:n]) wait(1) if(true)
        #pragma acc update host(f[0:n]) wait(2) if(true)
        #pragma acc update host(g[0:n]) wait(3) if(true)
    }

    for (int x = 0; x < n; ++x){
        if (fabs(c[x] - (a[x] + b[x])) > PRECISION){
	    err++;
	}
        if (fabs(f[x] - (d[x] + e[x])) > PRECISION){
            err++;
        }
        if (fabs(g[x] - (c[x] + f[x])) > PRECISION){
            err++;
        }
    }

    delete[] a;
    delete[] b;
    delete[] c;
    delete[] d;
    delete[] e;
    delete[] f;
    delete[] g;

    return err;
}
#endif
#ifndef T3
//T3:parallel,wait,async,V:2.7-3.2
int test3(){
    int err = 0;
    srand(time(NULL));

    real_t * a = new real_t[n];
    real_t * b = new real_t[n];
    data_container<real_t> c = *(new data_container<real_t>(n));
    real_t * d = new real_t[n];
    real_t * e = new real_t[n];
    data_container<real_t> f = *(new data_container<real_t>(n));
    data_container<real_t> g = *(new data_container<real_t>(n));

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c.data[x] = 0.0;
        d[x] = rand() / (real_t)(RAND_MAX / 10);
        e[x] = rand() / (real_t)(RAND_MAX / 10);
        f.data[x] = 0.0;
        g.data[x] = 0.0;
    }

   #pragma acc data copyin(a[0:n], b[0:n], c, c.data[0:n], d[0:n], e[0:n], f, f.data[0:n], g, g.data[0:n])
    {
        #pragma acc parallel loop async(1)
        for (int x = 0; x < n; ++x){
            c.data[x] = (a[x] + b[x]);
        }
        #pragma acc parallel loop async(2)
        for (int x = 0; x < n; ++x){
            f.data[x] = (d[x] + e[x]);
        }
        #pragma acc parallel loop wait(1, 2) async(3)
        for (int x = 0; x < n; ++x){
            g.data[x] = (c.data[x] + f.data[x]);
        }
        #pragma acc update host(c.data[0:n]) wait(1) if(true)
        #pragma acc update host(f.data[0:n]) wait(2) if(true)
        #pragma acc update host(g.data[0:n]) wait(3) if(true)
    }

    for (int x = 0; x < n; ++x){
        if (fabs(c.data[x] - (a[x] + b[x])) > PRECISION){
            err++;
        }
        if (fabs(f.data[x] - (d[x] + e[x])) > PRECISION){
            err++;
        }
        if (fabs(g.data[x] - (c.data[x] + f.data[x])) > PRECISION){
            err++;
        }
    }

    delete[] a;
    delete[] b;
    delete[] d;
    delete[] e;

    return err;
}
#endif
#ifndef T4
//T4:parallel,wait,async,V:2.7-3.2
int test4(){
    int err = 0;
    srand(time(NULL));

    data_container<real_t> a = *(new data_container<real_t>(n));
    data_container<real_t> b = *(new data_container<real_t>(n));
    real_t * c  = new real_t[n];
    data_container<real_t> d = *(new data_container<real_t>(n));
    data_container<real_t> e = *(new data_container<real_t>(n));
    real_t * f = new real_t[n];
    real_t * g = new real_t[n];

    for (int x = 0; x < n; ++x){
        a.data[x] = rand() / (real_t)(RAND_MAX / 10);
        b.data[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0.0;
        d.data[x] = rand() / (real_t)(RAND_MAX / 10);
        e.data[x] = rand() / (real_t)(RAND_MAX / 10);
        f[x] = 0.0;
        g[x] = 0.0;
    }

   #pragma acc data copyin(a,a.data[0:n],b,b.data[0:n],c[0:n],d,d.data[0:n],e,e.data[0:n], f[0:n], g[0:n])
    {
        #pragma acc parallel loop async(1)
        for (int x = 0; x < n; ++x){
            c[x] = (a.data[x] + b.data[x]);
        }
        #pragma acc parallel loop async(2)
        for (int x = 0; x < n; ++x){
            f[x] = (d.data[x] + e.data[x]);
        }
        #pragma acc parallel loop wait(1, 2) async(3)
        for (int x = 0; x < n; ++x){
            g[x] = (c[x] + f[x]);
        }
        #pragma acc update host(c[0:n]) wait(1) if(true)
        #pragma acc update host(f[0:n]) wait(2) if(true)
        #pragma acc update host(g[0:n]) wait(3) if(true)
    }

    for (int x = 0; x < n; ++x){
        if (fabs(c[x] - (a.data[x] + b.data[x])) > PRECISION){
            err++;
        }
        if (fabs(f[x] - (d.data[x] + e.data[x])) > PRECISION){
            err++;
        }
        if (fabs(g[x] - (c[x] + f[x])) > PRECISION){
            err++;
        }
    }

    delete[] c;
    delete[] f;
    delete[] g;

    return err;
}
#endif

int main(){
    int failcode = 0;
    int failed;
#ifndef T1
    failed = 0;
    for(int x = 0; x < NUM_TEST_CALLS; ++x){
	failed += test1();
    }
    if(failed){
	failcode += (1 << 0);
    }
#endif
#ifndef T2
    failed = 0;
    for(int x = 0; x < NUM_TEST_CALLS; ++x){
        failed += test2();
    }
    if(failed){
        failcode += (1 << 1);
    }
#endif
#ifndef T3
    failed = 0;
    for(int x = 0; x < NUM_TEST_CALLS; ++x){
        failed += test3();
    }
    if(failed){
        failcode += (1 << 2);
    }
#endif
#ifndef T4
    failed = 0;
    for(int x = 0; x < NUM_TEST_CALLS; ++x){
        failed += test4();
    }
    if(failed){
        failcode += (1 << 3);
    }
#endif
    return failcode;
}
