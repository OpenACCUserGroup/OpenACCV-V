#ifndef T1
!T1:runtime,async,construct-independent,V:2.0-2.7
      LOGICAL FUNCTION test1()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, b, c, d, e, f, g, h, i !Data
        REAL(8) :: RAND
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
        CALL RANDOM_NUMBER(g)
        h = 0
        i = 0

        !$acc data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT), d(1:LOOPCOUNT), e(1:LOOPCOUNT), g(1:LOOPCOUNT)) create(c(1:LOOPCOUNT), f(1:LOOPCOUNT), h(1:LOOPCOUNT)) copyout(i(1:LOOPCOUNT))
          !$acc parallel async(1)
            !$acc loop
              DO x = 1, LOOPCOUNT
                c(x) = a(x) + b(x)
              END DO
          !$acc end parallel
          CALL acc_wait_async(1, 2)
          !$acc parallel async(2)
            !$acc loop
            DO x = 1, LOOPCOUNT
              h(x) = c(x) + g(x)
            END DO
          !$acc end parallel
          !$acc parallel async(1)
            !$acc loop
            DO x = 1, LOOPCOUNT
              f(x) = d(x) + e(x)
            END DO
          !$acc end parallel
          CALL acc_wait_async(1, 2)
          !$acc parallel async(2)
            !$acc loop
            DO x = 1, LOOPCOUNT
              i(x) = h(x) + f(x)
            END DO
          !$acc end parallel
          !$acc wait(2)
        !$acc end data

        DO x = 1, LOOPCOUNT
          IF (abs(i(x) - (a(x) + b(x) + g(x) + d(x) + e(x))) .gt. PRECISION) THEN
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
