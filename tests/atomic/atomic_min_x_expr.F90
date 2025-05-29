#ifndef T1
!T1:construct-independent,atomic,V:2.0-2.7
      LOGICAL FUNCTION test1()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x, y !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a !Data
        REAL(8),DIMENSION(LOOPCOUNT/10 + 1):: totals, totals_comparison
        INTEGER :: errors = 0

        !Initilization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        CALL RANDOM_NUMBER(a)

        totals = 0
        totals_comparison = 0

        !$acc data copyin(a(1:LOOPCOUNT)) copy(totals(1:(LOOPCOUNT/10 + 1)))
          !$acc parallel
            !$acc loop
            DO x = 1, LOOPCOUNT
              !$acc atomic
                totals(MOD(x, LOOPCOUNT/10 + 1) + 1) = min(totals(MOD(x, LOOPCOUNT/10 + 1) + 1), a(x))
            END DO
          !$acc end parallel
        !$acc end data
        DO x = 1, LOOPCOUNT
          totals_comparison(MOD(x, LOOPCOUNT/10 + 1) + 1) = min(totals_comparison(MOD(x, LOOPCOUNT/10 + 1) + 1), a(x))
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
