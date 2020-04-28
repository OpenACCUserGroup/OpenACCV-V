#ifndef T1
!T1:async,serial,combined-constructs,loop,V:2.6-2.7
      LOGICAL FUNCTION test1()
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"
  INTEGER:: errors
  REAL(8),DIMENSION(LOOPCOUNT, 10):: a, b, c, d
  INTEGER,DIMENSION(10):: error_array
  INTEGER:: x, y

  errors = 0
  SEEDDIM(1) = 1
# ifdef SEED
  SEEDDIM(1) = SEED
# endif
  CALL RANDOM_SEED(PUT=SEEDDIM)

  CALL RANDOM_NUMBER(a)
  CALL RANDOM_NUMBER(b)
  c = 0

  DO y = 1, 10
    DO x = 1, LOOPCOUNT
      d(x, y) = a(x, y) + b(x, y)
    END DO
  END DO

  error_array = 0

  !$acc data copyin(a(1:LOOPCOUNT, 1:10), b(1:LOOPCOUNT, 1:10), c(1:LOOPCOUNT, 1:10), d(1:LOOPCOUNT, 1:10)) copy(error_array(1:10))
    DO y = 1, 10
      !$acc serial loop async(y)
      DO x = 1, LOOPCOUNT
        c(x, y) = a(x, y) + b(x, y)
      END DO
      !$acc serial loop async(y)
      DO x = 1, LOOPCOUNT
        IF (((c(x, y) - d(x, y)) .gt. PRECISION) .OR. ((d(x, y) - c(x, y)) .gt. PRECISION)) THEN
          error_array(y) = error_array(y) + 1
        END IF
      END DO
    END DO
  !$acc end data

  DO x = 1, 10
    errors = errors + error_array(x)
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
