#ifndef T1
!T1:data,data_region,construct-independent,V:1.0-2.7
      LOGICAL FUNCTION test1()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        TYPE multi_item
          REAL(8) :: a
          REAL(8) :: b
          REAL(8) :: c
        END TYPE multi_item
        INTEGER :: x !Iterators
        TYPE (multi_item),DIMENSION(LOOPCOUNT):: a !Data
        INTEGER :: errors = 0

        !Initilization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        DO x = 1, LOOPCOUNT
          CALL RANDOM_NUMBER(a(x)%a)
          CALL RANDOM_NUMBER(a(x)%b)
          a(x)%c = 0.0
        END DO

        !$acc data copy(a(1:LOOPCOUNT))
          !$acc parallel
            !$acc loop
            DO x = 1, LOOPCOUNT
              a(x)%c = a(x)%a + a(x)%b
            END DO
          !$acc end parallel
        !$acc end data
        DO x = 1, LOOPCOUNT
          IF (abs(a(x)%c - (a(x)%a + a(x)%b)) .gt. PRECISION) THEN
            errors = errors + 1
          END IF
        END DO

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
#ifndef T1
        LOGICAL :: test1
#endif
        failed = .FALSE.
        failcode = 0
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
