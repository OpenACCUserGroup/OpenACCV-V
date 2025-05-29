#ifndef T1
!T1:serial,loop,V:2.6-2.7
      LOGICAL FUNCTION test1()
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"
  REAL(8),DIMENSION(LOOPCOUNT):: a, b, c
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

  !$acc data copy(c(1:LOOPCOUNT))
    !$acc serial loop
    DO x = 1, LOOPCOUNT
      c(x) = 1
    END DO
  !$acc end data

  DO x = 1, LOOPCOUNT
    IF (abs(c(x) - 1) .gt. PRECISION) THEN
      errors = errors + 1
    END IF
  END DO

  !$acc data copy(a(1:LOOPCOUNT), b(1:LOOPCOUNT), c(1:LOOPCOUNT))
    !$acc serial
      !$acc loop
      DO x = 1, LOOPCOUNT
        c(x) = b(x)
      END DO
      !$acc loop
      DO x = 1, LOOPCOUNT
        c(x) = c(x) + a(x)
      END DO
    !$acc end serial
  !$acc end data

  DO x = 1, LOOPCOUNT
    IF (abs(c(x) - (a(x) + b(x))) .gt. PRECISION) THEN
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
