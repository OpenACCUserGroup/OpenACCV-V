#ifndef T1
!T1:loop,V:2.6-2.7
    LOGICAL FUNCTION test1()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        REAL(8),DIMENSION(LOOPCOUNT):: a, b, c
        INTEGER:: multiplier
        INTEGER:: x
        INTEGER:: errors

        errors = 0

        SEEDDIM(1) = 1
        # ifdef SEED
        SEEDDIM(1) = SEED
        # endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        c = 0

        !$acc data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) copyout(c(1:LOOPCOUNT))
            !$acc serial
            !$acc loop gang
            DO x = 1, LOOPCOUNT
                c(x) = (a(x) + b(x)) * multiplier
            END DO
            multiplier = multiplier + 1
            !$acc loop gang
            DO x = 1, LOOPCOUNT
                c(x) = c(x) + ((a(x) + b(x)) * multiplier)
            END DO
            !$acc end serial
        !$acc end data

        DO x = 1, LOOPCOUNT
            IF (abs(c(x) - (3 * (a(x) + b(x)))) .gt. PRECISION) THEN
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
