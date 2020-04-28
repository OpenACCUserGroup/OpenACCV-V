#ifndef T1
!T1:parallel,V:2.0-2.7
      LOGICAL FUNCTION test1()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x !Iterators
        INTEGER,DIMENSION(LOOPCOUNT):: a
        REAL(8),DIMENSION(LOOPCOUNT):: b, c, randoms!Data
        INTEGER :: errors = 0

        !Initilization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        CALL RANDOM_NUMBER(b)
        c = 0
        CALL RANDOM_NUMBER(randoms)
        a = floor(randoms * 3)

        !$acc enter data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT), c(1:LOOPCOUNT))
        !$acc parallel present(a(1:LOOPCOUNT), b(1:LOOPCOUNT), c(1:LOOPCOUNT))
          !$acc loop
          DO x = 1, LOOPCOUNT
            SELECT CASE (a(x))
              CASE (0)
                c(x) = b(x) * b(x)
              CASE (1)
                c(x) = b(x) / b(x)
              CASE (2)
                c(x) = b(x) / 2
            END SELECT
          END DO
        !$acc end parallel
        !$acc exit data delete(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) copyout(c(1:LOOPCOUNT))

        DO x = 1, LOOPCOUNT
          SELECT CASE (a(x))
            CASE (0)
              IF (abs(c(x) - (b(x) * b(x))) .gt. PRECISION) THEN
                errors = errors + 1
              END IF
            CASE (1)
              IF (abs(c(x) - (b(x) / b(x))) .gt. PRECISION) THEN
                errors = errors + 1
              END IF
            CASE (2)
              IF (abs(c(x) - (b(x) / 2)) .gt. PRECISION) THEN
                errors = errors + 1
              END IF
          END SELECT
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
