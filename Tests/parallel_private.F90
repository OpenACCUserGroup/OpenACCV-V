#ifndef T1
!T1:parallel,private,V:2.0-2.7
      LOGICAL FUNCTION test1()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x, y !Iterators
        REAL(8),DIMENSION(10 * LOOPCOUNT):: a, b !Data
        REAL(8),DIMENSION(LOOPCOUNT):: c
        REAL(8),DIMENSION(10)::d
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
        d = 0

        !$acc enter data copyin(a(1:10*LOOPCOUNT), b(1:10*LOOPCOUNT), d(1:10)) 
        !$acc parallel num_gangs(10) private(c(1:LOOPCOUNT))
          !$acc loop gang
          DO x = 1, 10
            !$acc loop worker
            DO y = 1, LOOPCOUNT
              c(y) = a((x - 1) * LOOPCOUNT + y) + b((x - 1) * LOOPCOUNT + y)
            END DO
            !$acc loop seq
            DO y = 1, LOOPCOUNT
              d(x) = d(x) + c(y)
            END DO
          END DO
        !$acc end parallel
        !$acc exit data copyout(d(1:10)) delete(a(1:10*LOOPCOUNT), b(1:10*LOOPCOUNT))

        DO x = 0, 9
          temp = 0
          DO y = 1, LOOPCOUNT
            temp = temp + a(x * LOOPCOUNT + y) + b(x * LOOPCOUNT + y)
          END DO
          IF (abs(temp - d(x + 1)) .gt. 2 * PRECISION * LOOPCOUNT) THEN
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
