#ifndef T1
!T1:runtime,construct-independent,internal-control-values,shutdown,nonvalidating,V:1.0-2.7
      LOGICAL FUNCTION test1()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        IF (acc_get_device_type() .ne. acc_device_none) THEN
          CALL acc_shutdown(acc_get_device_type())
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