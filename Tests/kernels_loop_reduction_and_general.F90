#ifndef T1
!T1:kernels,reduction,combined-constructs,loop,V:1.0-2.7
      LOGICAL FUNCTION test1()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: randoms
        LOGICAL,DIMENSION(LOOPCOUNT):: a !Data
        LOGICAL :: results = .TRUE.
        LOGICAL :: host_results = .TRUE.
        REAL(8) :: false_margin
        INTEGER :: errors = 0
        false_margin = exp(log(.5) / LOOPCOUNT)
        !Initilization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        CALL RANDOM_NUMBER(randoms)
        DO x = 1, LOOPCOUNT
          IF (randoms(x) .lt. false_margin) THEN
            a(x) = .TRUE.
          ELSE
            a(x) = .FALSE.
          END IF
        END DO

        !$acc data copyin(a(1:LOOPCOUNT))
          !$acc kernels loop reduction(.and.:results)
          DO x = 1, LOOPCOUNT
            results = results .and. a(x)
          END DO
        !$acc end data

        DO x = 1, LOOPCOUNT
          host_results = host_results .and. a(x)
        END DO
        IF (host_results .neqv. results) THEN
          errors = 1
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
