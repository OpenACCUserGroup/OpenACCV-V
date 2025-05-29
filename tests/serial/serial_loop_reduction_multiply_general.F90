#ifndef T1
!T1:serial,reduction,combined-constructs,loop,V:2.6-2.7
      LOGICAL FUNCTION test1()
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"
  REAL(8),DIMENSION(10):: a, b
  REAL(8):: reduced, host_reduced
  INTEGER:: errors, x, y

  errors = 0
  SEEDDIM(1) = 1
# ifdef SEED
  SEEDDIM(1) = SEED
# endif
  CALL RANDOM_SEED(PUT=SEEDDIM)

  DO y = 1, LOOPCOUNT
    CALL RANDOM_NUMBER(a)
    CALL RANDOM_NUMBER(b)
    reduced = 1
    host_reduced = 1

    DO x = 1, 10
      host_reduced = host_reduced * (a(x) + b(x))
    END DO

    !$acc data copyin(a(1:10), b(1:10))
      !$acc serial loop reduction(*:reduced)
      DO x = 1, 10
        reduced = reduced * (a(x) + b(x))
      END DO
    !$acc end data

    IF (abs(host_reduced - reduced) .gt. PRECISION) THEN
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
