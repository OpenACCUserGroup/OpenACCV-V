#ifndef T1
!T1:serial,private,V:2.6-2.7
      LOGICAL FUNCTION test1()
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"
  REAL(8),DIMENSION(LOOPCOUNT, 10):: a, b
  REAL(8),DIMENSION(LOOPCOUNT):: c
  REAL(8),DIMENSION(10):: d
  REAL(8):: temp
  INTEGER:: x, y
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
  d = 0

  !$acc data copyin(a(1:LOOPCOUNT, 1:10), b(1:LOOPCOUNT, 1:10)) copy(d(1:10))
    !$acc serial private(c(1:LOOPCOUNT))
      !$acc loop gang
      DO y = 1, 10
        !$acc loop worker
        DO x = 1, LOOPCOUNT
          c(x) = a(x, y) + b(x, y)
        END DO
        !$acc loop seq
        DO x = 1, LOOPCOUNT
          d(y) = d(y) + c(x)
        END DO
      END DO
    !$acc end serial
  !$acc end data

  DO y = 1, 10
    temp = 0
    DO x = 1, LOOPCOUNT
      temp = temp + (a(x, y) + b(x, y))
    END DO
    IF (abs(d(x) - temp) .gt. (2 * PRECISION * LOOPCOUNT)) THEN
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
