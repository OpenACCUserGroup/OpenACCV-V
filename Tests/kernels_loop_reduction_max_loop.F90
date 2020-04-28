#ifndef T1
!T1:kernels,private,reduction,combined-constructs,loop,V:1.0-2.7
      LOGICAL FUNCTION test1()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x, y !Iterators
        REAL(8),DIMENSION(10 * LOOPCOUNT):: a, b, c !Data
        REAL(8),DIMENSION(10):: maximum
        REAL(8) :: temp
        INTEGER :: errors = 0

        !Initilization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        c = 0

        !$acc data copyin(a(1:10*LOOPCOUNT), b(1:10*LOOPCOUNT)) copy(c(1:10*LOOPCOUNT))
          !$acc kernels loop gang private(temp)
          DO x = 0, 9
            temp = 0
            !$acc loop worker reduction(max:temp)
            DO y = 1, LOOPCOUNT
              temp = max(temp, a(x * LOOPCOUNT + y) * b(x * LOOPCOUNT + y))
            END DO
            maximum(x + 1) = temp
            !$acc loop worker
            DO y = 1, LOOPCOUNT
              c(x * LOOPCOUNT + y) = (a(x * LOOPCOUNT + y) * b(x * LOOPCOUNT + y)) / maximum(x + 1)
            END DO
          END DO
        !$acc end data

        DO x = 0, 9
          DO y = 1, LOOPCOUNT
            IF (a(x * LOOPCOUNT + y) * b(x * LOOPCOUNT + y) - maximum(x + 1) .gt. PRECISION) THEN
              errors = errors + 1
            ELSE IF ((c(x * LOOPCOUNT + y) - 1) .gt. PRECISION) THEN
              errors = errors + 1
            END IF
          END DO
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
