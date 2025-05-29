#ifndef T1
!T1:runtime,construct-independent,internal-control-values,present,V:1.0-2.7
      LOGICAL FUNCTION test1()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: errors
        INTEGER :: device_type
        errors = 0
        device_type = acc_get_device_type()
        IF (device_type .ne. acc_device_none) THEN
          !$acc parallel
            IF (acc_on_device(device_type) .eqv. .FALSE.) THEN
              errors = errors + 1
            END IF
          !$acc end parallel
        ELSE
          !$acc parallel
            IF (acc_on_device(acc_device_host) .eqv. .FALSE.) THEN
              errors = errors + 1
            END IF
          !$acc end parallel
        END IF

        IF (errors .eq. 0) THEN
          test1 = .FALSE.
        ELSE
          test1 = .TRUE.
        END IF
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

