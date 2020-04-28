#ifndef T1
!T1:async,parallel,update,V:2.0-2.7
      LOGICAL FUNCTION test1()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x, y, z !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, b, c, d, e, f, g !Data
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
        CALL RANDOM_NUMBER(d)
        CALL RANDOM_NUMBER(e)
        f = 0
        g = 0
        
        !$acc data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT), d(1:LOOPCOUNT), e(1:LOOPCOUNT), c(1:LOOPCOUNT), f(1:LOOPCOUNT), g(1:LOOPCOUNT))
          !$acc parallel async(1)
            !$acc loop
            DO x = 1, LOOPCOUNT
              c(x) = a(x) + b(x)
            END DO
          !$acc end parallel
          !$acc parallel async(2)
            !$acc loop
            DO x = 1, LOOPCOUNT
              f(x) = d(x) + e(x)
            END DO
          !$acc end parallel
          !$acc update host(c(1:LOOPCOUNT)) wait(1) async(4)
          !$acc update host(f(1:LOOPCOUNT)) wait(2) async(5)
          !$acc parallel wait(1, 2) async(3)
            !$acc loop
            DO x = 1, LOOPCOUNT
              g(x) = c(x) + f(x)
            END DO
          !$acc end parallel
          !$acc update host(g(1:LOOPCOUNT)) async(3)
        !$acc end data
        !$acc wait
        DO x = 1, LOOPCOUNT
          IF (abs(c(x) - (a(x) + b(x))) .gt. PRECISION) THEN
            errors = errors + 1
          ELSE IF (abs(f(x) - (d(x) + e(x))) .gt. PRECISION) THEN
            errors = errors + 1
          ELSE IF (abs(g(x) - (f(x) + c(x))) .gt. PRECISION) THEN
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
