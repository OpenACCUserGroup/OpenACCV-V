#ifndef T1
!T1:kernels,V:2.5-2.7
      LOGICAL FUNCTION test1()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, b !Data
        INTEGER :: errors = 0
        REAL(8) :: total = 0.0
        !Initilization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)

        DO x = 1, LOOPCOUNT
          total = total + a(x)
        END DO
        !$acc data copyin(b(1:LOOPCOUNT))
          !$acc kernels num_gangs(1) vector_length(1)
            !$acc loop
            DO x = 1, LOOPCOUNT
              total = total + b(x)
            END DO
          !$acc end kernels
        !$acc end data

        DO x = 1, LOOPCOUNT
          total = total - (a(x) + b(x))
        END DO
        IF (abs(total) .gt. (2 * LOOPCOUNT - 2) * PRECISION) THEN
          errors = errors + 1
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
