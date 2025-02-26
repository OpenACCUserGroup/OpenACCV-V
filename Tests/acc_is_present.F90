#include "common.Fh"

#ifndef T1
!T1:runtime,devonly,construct-independent,present,V:2.0-2.7
      LOGICAL FUNCTION test1()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a !Data
        INTEGER :: errors
        errors = 0

        !$acc enter data create(a(1:LOOPCOUNT))
        IF (acc_is_present(a(1:LOOPCOUNT)) .eqv. .FALSE.) THEN
          errors = errors + 1
          PRINT*, 1
        END IF
        !$acc exit data delete(a(1:LOOPCOUNT))

        IF (devtest() .eq. .TRUE.) THEN
          IF (acc_is_present(a(1:LOOPCOUNT)) .eqv. .TRUE.) THEN
            errors = errors + 1
            PRINT*, 2
          END IF
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

