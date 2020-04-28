#ifndef T1
!T1:runtime,construct-independent,internal-control-values,nonvalidating,V:2.6-2.7
      LOGICAL FUNCTION test1()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: output
        IF (acc_get_device_type() .ne. acc_device_none) THEN
          output = acc_get_property(acc_get_device_num(acc_get_device_type()), acc_get_device_type(), acc_property_memory)
          output = acc_get_property(acc_get_device_num(acc_get_device_type()), acc_get_device_type(), acc_property_free_memory)
        END IF
        test1 = .FALSE.
      END
#endif


      PROGRAM main
        IMPLICIT NONE
        INTEGER :: failcode, testrun
        LOGICAL :: failed
        INCLUDE "acc_testsuite.Fh"
        !Conditionally define test functions
#ifndef T1
        LOGICAL :: test1
#endif
        failcode = 0
        failed = .FALSE.

#ifndef T1
        DO testrun = 1, NUM_TEST_CALLS
          failed = failed .or. test1()
        END DO
        IF (failed) THEN
          failcode = failcode + 2 ** 0
          failed = .FALSE.
        END IF
#endif
        CALL EXIT (failcode)
      END PROGRAM

