#include "acc_testsuite.h"

//test 1 host lambda
#pragma acc routine vector bind(device_array_array)
auto  host_array_array = [](real_t * a, long long n){
    #pragma acc loop reduction(+:returned)
    real_t returned = 0.0;
    for (int x = 0; x < n; ++x){
        returned += a[x];
    }
    return returned;
};

//test 1 device function
real_t device_array_array(real_t * a, long long n){
    real_t returned = 0.0;
    #pragma acc loop reduction(-:returned)
    for (int x = 0; x < n; ++x){
        returned -= a[x];
    }
    return returned;
}

//test 2 host lambda
#pragma acc routine vector bind(device_object_array)
auto host_object_array= [](data_container<real_t> * a, long long n){
    real_t returned = 0.0;
    #pragma acc loop reduction(-:returned)
    for(int x = 0; x < n; ++x){
	returned += a->data[x];
    }
    return returned;
};

//test 2 device function
real_t device_object_array(data_container<real_t> *a, long long n){
    real_t returned = 0.0;
    #pragma acc loop reduction(-:returned)
    for(int x = 0; x < n; ++x){
        returned -= a->data[x];
    }
    return returned;
}

//test 3 host lambda
#pragma acc routine vector bind(device_array_object)
auto host_array_object = [](real_t * a, long long n){
   #pragma acc loop reduction(+:returned)
    real_t returned = 0.0;
    for (int x = 0; x < n; ++x){
        returned += a[x];
    }
    return returned;
};

//test 3 device function
real_t device_array_object(real_t * a, long long n){
    real_t returned = 0.0;
    #pragma acc loop reduction(-:returned)
    for (int x = 0; x < n; ++x){
        returned -= a[x];
    }
    return returned;
}

//test 4 host lambda
#pragma acc routine vector bind(device_object_object)
auto host_object_object = [](data_container<real_t> * a, long long n){
    real_t returned = 0.0;
    #pragma acc loop reduction(-:returned)
    for(int x = 0; x < n; ++x){
        returned += a->data[x];
    }
    return returned;
};

//test 4 device function
real_t device_object_object(data_container<real_t> *a, long long n){
    real_t returned = 0.0;
    #pragma acc loop reduction(-:returned)
    for(int x = 0; x < n; ++x){
        returned -= a->data[x];
    }
    return returned;
}

#ifndef T1
//T1:routine,V:2.7-3.3
int test1(){
    int err = 0;
    srand(SEED);
    real_t *a = new real_t[n];
    real_t *b = new real_t[n];
    int on_host = (acc_get_device_type() == acc_device_none);

    for (int x = 0; x < n; ++x){
       	a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = 0;
    }

    #pragma acc data copyin(a[0:n]) copyout(b[0:n])
    {
        #pragma acc parallel
        {
            #pragma acc loop gang worker vector
            for (int x = 0; x < n; ++x){
                b[x] = device_array_array(a, n);
            }
        }
    }

    for (int x = 0; x < n; ++x){
        if ((!on_host) && (fabs(host_array_array(a, n) + b[x]) > PRECISION)){
            err += 1;
        }
        else if ((on_host) && (fabs(host_array_array(a, n) - b[x]) > PRECISION)){
            err += 1;
        }
    }
    delete[] a;
    delete[] b;

    return err;
}
#endif
#ifndef T2
//T2:routine,V:2.7-3.3
int test2(){
    int err = 0;
    srand(SEED);
    data_container<real_t> a = *(new data_container<real_t>(n));
    real_t *b = new real_t[n];
    int on_host = (acc_get_device_type() == acc_device_none);

    for (int x = 0; x < n; ++x){
        a.data[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = 0;
    }

    #pragma acc data copyin(a, a.data[0:n]) copyout(b[0:n])
    {
        #pragma acc parallel
        {
            #pragma acc loop gang worker vector
            for (int x = 0; x < n; ++x){
                b[x] = device_object_array(&a, n);
            }
        }
    }

    for (int x = 0; x < n; ++x){
        if ((!on_host) && (fabs(host_object_array(&a, n) + b[x]) > PRECISION)){
            err += 1;
        }
        else if ((on_host) && (fabs(host_object_array(&a, n) - b[x]) > PRECISION)){
            err += 1;
        }
    }

    delete[] b;

    return err;
}
#endif
#ifndef T3
//T3:routine,V:2.7-3.3
int test3(){
    int err = 0;
    srand(SEED);
    real_t *a = new real_t[n];
    data_container<real_t> b = *(new data_container<real_t>(n));
    int on_host = (acc_get_device_type() == acc_device_none);

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b.data[x] = 0.0;
    }

    #pragma acc data copyin(a[0:n], b, b.data[0:n])
    {
        #pragma acc parallel
        {
            #pragma acc loop gang worker vector
            for (int x = 0; x < n; ++x){
                b.data[x] = device_array_object(a, n);
            }
        }
	#pragma acc update host(b.data[0:n])
    }

    for (int x = 0; x < n; ++x){
        if ((!on_host) && (fabs(host_array_object(a, n) + b.data[x]) > PRECISION)){
            err += 1;
        }
        else if ((on_host) && (fabs(host_array_object(a, n) - b.data[x]) > PRECISION)){
            err += 1;
        }
    }

    delete[] a;

    return err;
}
#endif
#ifndef T4
//T4:routine,V:2.7-3.3
int test4(){
    int err = 0;
    srand(SEED);
    data_container<real_t> a = *(new data_container<real_t>(n));
    data_container<real_t> b = *(new data_container<real_t>(n));
    int on_host = (acc_get_device_type() == acc_device_none);

    for (int x = 0; x < n; ++x){
        a.data[x] = rand() / (real_t)(RAND_MAX / 10);
        b.data[x] = 0.0;
    }

    #pragma acc data copyin(a, a.data[0:n], b ,b.data[0:n])
    {
        #pragma acc parallel
        {
            #pragma acc loop gang worker vector
            for (int x = 0; x < n; ++x){
                b.data[x] = device_object_object(&a, n);
            }
        }
	#pragma acc update host(b.data[0:n])
    }

    for (int x = 0; x < n; ++x){
        if ((!on_host) && (fabs(host_object_object(&a, n) + b.data[x]) > PRECISION)){
            err += 1;
        }
        else if ((on_host) && (fabs(host_object_object(&a, n) - b.data[x]) > PRECISION)){
            err += 1;
        }
    }

    return err;
}
#endif

int main(){
    int failcode = 0;
    int failed = 0;
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
