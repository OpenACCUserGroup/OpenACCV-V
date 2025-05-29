#ifndef T1
!T1:construct-independent,atomic,V:2.0-2.7
      LOGICAL FUNCTION test1()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x, y !Iterators
        INTEGER,DIMENSION(LOOPCOUNT):: a !Data
        REAL(8),DIMENSION(LOOPCOUNT, 8):: randoms
        INTEGER,DIMENSION(LOOPCOUNT/10 + 1):: totals, totals_comparison
        INTEGER :: errors = 0

        !Initilization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        CALL RANDOM_NUMBER(randoms)
        a = 0
        DO x = 1, LOOPCOUNT
          DO y = 1, 8
            IF (randoms(x, y) .lt. .933) THEN
              a(x) = a(x) + ISHFT(1, y - 1)
            END IF
          END DO
        END DO
        totals = 0
        totals_comparison = 0
        DO x = 1, LOOPCOUNT/10 + 1
          totals(x) = 0
          totals_comparison(x) = 0
        END DO

        !$acc data copyin(a(1:LOOPCOUNT)) copy(totals(1:(LOOPCOUNT/10 + 1)))
          !$acc parallel
            !$acc loop
            DO x = 1, LOOPCOUNT
              !$acc atomic update
                totals(MOD(x, LOOPCOUNT/10 + 1) + 1) = ior(totals(MOD(x, LOOPCOUNT/10 + 1) + 1), a(x))
              !$acc end atomic
            END DO
          !$acc end parallel
        !$acc end data
        DO x = 1, LOOPCOUNT
          totals_comparison(MOD(x, LOOPCOUNT/10 + 1) + 1) = ior(totals_comparison(MOD(x, LOOPCOUNT/10 + 1) + 1), a(x))
        END DO
        DO x = 1, LOOPCOUNT/10 + 1
          IF (totals_comparison(x) .NE. totals(x)) THEN
            errors = errors + 1
            WRITE(*, *) totals_comparison(x)
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
