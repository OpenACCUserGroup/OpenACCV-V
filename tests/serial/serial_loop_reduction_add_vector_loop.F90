#ifndef T1
!T1:serial,private,reduction,combined-constructs,loop,V:2.6-2.7
      LOGICAL FUNCTION test1()
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"
  INTEGER:: errors
  REAL(8),DIMENSION(LOOPCOUNT, 10):: a, b
  REAL(8),DIMENSION(10):: c
  REAL(8):: temp
  INTEGER:: x, y

  errors = 0

  SEEDDIM(1) = 1
# ifdef SEED
  SEEDDIM(1) = SEED
# endif
  CALL RANDOM_SEED(PUT=SEEDDIM)

  CALL RANDOM_NUMBER(a)
  CALL RANDOM_NUMBER(b)

  !$acc data copyin(a(1:LOOPCOUNT,1:10), b(1:LOOPCOUNT,1:10)) copyout(c(1:10))
    !$acc serial loop private(temp)
    DO y = 1, 10
      temp = 0
      !$acc loop vector reduction(+:temp)
      DO x = 1, LOOPCOUNT
        temp = temp + (a(x, y) + b(x, y))
      END DO
      c(y) = temp
    END DO
  !$acc end data

  DO y = 1, 10
    DO x = 1, LOOPCOUNT
      c(y) = c(y) - (a(x, y) + b(x, y))
    END DO
    IF (abs(c(y)) .gt. PRECISION) THEN
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
