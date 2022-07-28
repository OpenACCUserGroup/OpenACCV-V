#ifndef T1
!T1:serial,private,reduction,combined-constructs,loop,V:2.6-2.7
      LOGICAL FUNCTION test1()
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"
  REAL(8),DIMENSION(LOOPCOUNT, 10):: a, b, c
  REAL(8),DIMENSION(10):: minimums, host_minimums
  REAL(8):: temp
  INTEGER:: errors, x, y

  errors = 0

  SEEDDIM(1) = 1
  #ifdef SEED
  SEEDDIM(1) = SEED
  #endif
  CALL RANDOM_SEED(PUT=SEEDDIM)

  CALL RANDOM_NUMBER(a)
  CALL RANDOM_NUMBER(b)
  c = 0
  minimums = 0
  host_minimums = 0

  DO y = 1, 10
    DO x = 1, LOOPCOUNT
      host_minimums(y) = min(host_minimums(y), a(x, y) * b(x, y))
    END DO
  END DO

  !$acc data copyin(a(1:LOOPCOUNT, 1:10), b(1:LOOPCOUNT, 1:10)) copy(c(1:LOOPCOUNT, 1:10), minimums(1:10))
    !$acc serial loop gang private(temp)
    DO y = 1, 10
      temp = 1
      !$acc loop reduction(min: temp)
      DO x = 1, LOOPCOUNT
        temp = min(temp, a(x, y) * b(x, y))
      END DO
      minimums(y) = temp
      !$acc loop
      DO x = 1, LOOPCOUNT
        c(x, y) = (a(x, y) * b(x, y)) / minimums(y)
      END DO
    END DO
  !$acc end data

  DO y = 1, 10
    IF (abs(minimums(y) - host_minimums(y)) .gt. PRECISION) THEN
      errors = errors + 1
    END IF
    DO x = 1, LOOPCOUNT
      IF (abs(c(x, y) - ((a(x, y) * b(x, y)) / minimums(y))) .gt. PRECISION) THEN
        errors = errors + 1
      END IF
    END DO
  END DO

        IF (errors .eq. 0) THEN
          test1 = .FALSE.
        ELSE
          test1 = .TRUE.
        END IF
      END
#endif

      PROGRAM serial_loop_reduction_min_loop
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
