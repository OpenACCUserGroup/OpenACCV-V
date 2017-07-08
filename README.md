# OpenACCV-V
This repository will be updated with OpenACC test codes validating and verifying implementations of features and its conformance to the OpenACC specification. Currently work is under progress to make the suite publicly available, please check by around early September, 2017. For any further questions, please feel free to contact Sunita Chandrasekaran, University of Delaware @ schandra@udel.edu. 

Meanwhile here is a link to the project presentation "OpenACC 2.5 Validation Testsuite targeting multiple architectures" http://www.csm.ornl.gov/workshops/p3ma2017/documents/Chandrasekaran_OpenACCV_V2.5.pdf at the 2nd International Workshop on Performance Portable Programming Models for Accelerators (P^3MA) co-located with ISC, Germany, 2017. The paper will be published within the Springer LNCS series in a few months time as post workshop proceedings. 

This testsuite is meant to test the functionality of the OpenACC language features.  The tests are decomposed to
test each small segment of the functionality of the language as described in the OpenACC specification
V2.5.  In order to run the suite, we have provided some assisting scripts that compile, run, and report
on each of the tests or a specific test if one is provided.  These are intended to assist you, but they
are not built to work comprehensively every situation and platform.  If these scripts don't provide the
needed functionality or fail to work on your system, the test suite can run independently. You would simply need to compile
the test that you are running, using the necessary compiler flags to enable OpenACC compilation
directives, and run it.  It will display the performance of the test, either passing or failing.

While not implemented in this version, an update is planned for release by the end of September of 2017
to have thorough documentation, including both OpenACC specifications to test mapping and test to
OpenACC specifications mapping.  Also, detailed descriptions of each of the tests will be outlined within each of the tests in the suite. 
For the reverse mapping, we plan on hyperlinking paragraphs of the specifications to a self-generating
html page that shows the list of related tests.  The test-to-OpenACC-specifications mapping will look
at least similar to this example of the acc_copyin.c test's documentation:


/*
This tests the following functionality of acc_copyin runtime routine:
Line 1995: The formatting of the runtime routine (incomplete testing, see note 1)
Lines 1996-1997: The equivalence to an "#pragma acc enter data copyin()" (incomplete testing, see note 2)
Lines 954-956: The transfer of data is tested.
Line 1997: The argument specifications
Lines 1999-2001: Reference counting (incomplete testing, see note 3)
Lines 2002-2003: Tests that the data is coppied and that reference counts are set to one.


This test is missing the following functionality:
Line 1998: The returned value is not tested nor used
Lines 2001-2002: The host version of the routine is not tested
Line 2004: The returned value is not tested nor used
Lines 2004-2006: The returned value is not used in a deviceptr clause
Lines 2014-2017: The async versions are not tested
Lines 2018-2019: The compatibility versions are not tested

Notes:
1:
The test only uses the first of the two formats of C/C++ formats for this routine.  Additional testing should be added to both testing the formatting for the second type, as well as the asyncronous functionality of the second format as well.

2:
This line should imply that all testing that is done on an "#pragma acc enter data copyin()" should be done on the runtime routine as well which is not yet implemented such as:
Lines 952-954: Reference counting with enter data directive

3:
There should be more extensive testing that tests more versions of reference counting scenarios.

Dependencies:
Shared:
data copy
enter data copyin
exit data copyout
parallel present
loop
Separate:
data copy
data copyin copyout
exit data copyout
parallel present
loop
*/
