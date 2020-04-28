#ifndef T1
!T1:serial,V:2.6-2.7
      LOGICAL FUNCTION test1()
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"
  INTEGER:: errors
  REAL(8),DIMENSION(LOOPCOUNT):: a, b, c
  INTEGER:: x
  errors = 0

  SEEDDIM(1) = 1
# ifdef SEED
  SEEDDIM(1) = SEED
# endif
  CALL RANDOM_SEED(PUT=SEEDDIM)

  CALL RANDOM_NUMBER(a)
  b = 0
  c = 0

  !$acc data copyin(a(1:LOOPCOUNT)) copy(c(1:LOOPCOUNT))
    !$acc serial create(b(1:LOOPCOUNT))
      !$acc loop
      DO x = 1, LOOPCOUNT
        b(x) = a(x)
      END DO
      !$acc loop
      DO x = 1, LOOPCOUNT
        c(x) = b(x)
      END DO
    !$acc end serial
  !$acc end data

  DO x = 1, LOOPCOUNT
    IF (abs(a(x) - c(x)) .gt. PRECISION) THEN
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
