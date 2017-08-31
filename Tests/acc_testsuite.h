/* Global headerfile of the OpenACC Testsuite */

/* This file was created with the accts_makeHeader.pl script using the following opions: */
/* -f=accts-c.conf -t=c  */


#ifndef ACC_TESTSUITE_H
#define ACC_TESTSUITE_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#ifdef _OPENACC
#include <openacc.h>
#endif
#include <string.h>
/* Version info                                           */
/**********************************************************/
#define ACCTS_VERSION "2.5"


/* General                                                */
/**********************************************************/
#define ARRAYSIZE   1000
long long n = ARRAYSIZE;
#define PRECISION   1e-8
typedef double real_t;
typedef enum { false, true } bool;
#define ARRAYSIZE_NEW 1024
#define ARRAYSIZE_SMALL 10

#define REPETITIONS 1
#define LOOPCOUNT 1000
/* following times are in seconds */
#define SLEEPTIME	 0.01
#define SLEEPTIME_LONG	 0.5

typedef struct {
  double real;
  double imag;
} dcomplex;

int test_acc_kernels_default_none(FILE * logfile);  /* Test for acc kernels default_none */
int crosstest_acc_kernels_default_none(FILE * logfile);  /* Crosstest for acc kernels default_none */
int test_acc_kernels_device_type(FILE * logfile);  /* Test for acc kernels device_type */
int crosstest_acc_kernels_device_type(FILE * logfile);  /* Crosstest for acc kernels device_type */
int test_acc_kernels_wait(FILE * logfile);  /* Test for acc kernels wait */
int crosstest_acc_kernels_wait(FILE * logfile);  /* Crosstest for acc kernels wait */
int test_acc_parallel_default_none(FILE * logfile);  /* Test for acc parallel default_none */
int crosstest_acc_parallel_default_none(FILE * logfile);  /* Crosstest for acc parallel default_none */
int test_acc_parallel_device_type(FILE * logfile);  /* Test for acc parallel device_type */
int crosstest_acc_parallel_device_type(FILE * logfile);  /* Crosstest for acc parallel device_type */
int test_acc_parallel_wait(FILE * logfile);  /* Test for acc parallel wait */
int crosstest_acc_parallel_wait(FILE * logfile);  /* Crosstest for acc parallel wait */
int test_acc_parallel_firstprivate(FILE * logfile);  /* Test for acc parallel firstprivate */
int crosstest_acc_parallel_firstprivate(FILE * logfile);  /* Crosstest for acc parallel firstprivate */

#endif
