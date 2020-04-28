#ifndef T1
!T1:construct-independent,loop,V:1.0-2.7
      LOGICAL FUNCTION test1()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x, y !Iterators
        REAL(8),DIMENSION(10*LOOPCOUNT):: a, b, c !Data
        REAL(8):: total
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

        DO x = 0, 9
          c(x*LOOPCOUNT + 1) = a(x*LOOPCOUNT + 1) + b(x*LOOPCOUNT + 1)
        END DO

        !$acc data copyin(a(1:10*LOOPCOUNT), b(1:10*LOOPCOUNT)) copy(c(1:10*LOOPCOUNT))
          !$acc parallel
            !$acc loop independent
            DO x = 0, 9
              DO y = 2, LOOPCOUNT
                c(x*LOOPCOUNT + y) = c(x*LOOPCOUNT + y - 1) + a(x*LOOPCOUNT + y) + b(x*LOOPCOUNT + y)
              END DO
            END DO
          !$acc end parallel
        !$acc end data
        DO x = 0, 9
          total = 0
          DO y = 1, LOOPCOUNT
            total = total + a(x * LOOPCOUNT + y) + b(x * LOOPCOUNT + y)
            IF (abs(total - c(x * LOOPCOUNT + y)) .gt. PRECISION) THEN
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
#ifndef T2
!T2:construct-independent,loop,V:1.0-2.7
      LOGICAL FUNCTION test2()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x, y !Iterators
        REAL(8),DIMENSION(10*LOOPCOUNT):: a, b, c !Data
        REAL(8):: total
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

        !$acc data copyin(a(1:10*LOOPCOUNT), b(1:10*LOOPCOUNT)) copyout(c(1:10*LOOPCOUNT))
          !$acc parallel
            !$acc loop independent collapse(2)
            DO x = 0, 9
              DO y = 1, LOOPCOUNT
                c(x*LOOPCOUNT + y) = a(x*LOOPCOUNT + y) + b(x*LOOPCOUNT + y)
              END DO
            END DO
          !$acc end parallel
        !$acc end data

        DO x = 0, 9
          DO y = 1, LOOPCOUNT
            IF (abs(c(x*LOOPCOUNT+y) - (a(x*LOOPCOUNT+y)+b(x*LOOPCOUNT+y))) .gt. PRECISION) THEN
              errors = errors + 1
            END IF
          END DO
        END DO

        IF (errors .eq. 0) THEN
          test2 = .FALSE.
        ELSE
          test2 = .TRUE.
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
#ifndef T2
        LOGICAL :: test2
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
#ifndef T2
        DO testrun = 1, NUM_TEST_CALLS
          failed = failed .or. test2()
        END DO
        IF (failed) THEN
          failcode = failcode + 2 ** 1
          failed = .FALSE.
        END IF
#endif
        CALL EXIT (failcode)
      END PROGRAM
