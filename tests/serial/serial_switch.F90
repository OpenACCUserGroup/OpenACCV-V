#ifndef T1
!T1:,V:2.6-2.7
      LOGICAL FUNCTION test1()
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"
  INTEGER,DIMENSION(LOOPCOUNT):: a
  REAL(8),DIMENSION(LOOPCOUNT):: a_randoms, b, b_host, c
  INTEGER:: errors, x

  errors = 0

  SEEDDIM(1) = 1
# ifdef SEED
  SEEDDIM(1) = SEED
# endif
  CALL RANDOM_SEED(PUT=SEEDDIM)

  CALL RANDOM_NUMBER(a_randoms)
  CALL RANDOM_NUMBER(b)
  c = 0
  b_host = b

  DO x = 1, LOOPCOUNT
    a(x) = INT(a_randoms(x) * 4)
  END DO

  !$acc data copyin(a(1:LOOPCOUNT)) copy(b(1:LOOPCOUNT), c(1:LOOPCOUNT))
    !$acc serial
      !$acc loop
      DO x = 1, LOOPCOUNT
        SELECT CASE(a(x))
        CASE (0)
          c(x) = b(x) * b(x)
        CASE (1)
          c(x) = b(x) / b(x)
        CASE (2)
          b(x) = b(x) / 2
        CASE DEFAULT
          c(x) = a(x) + b(x)
        END SELECT
      END DO
    !$acc end serial
  !$acc end data

  DO x = 1, LOOPCOUNT
    SELECT CASE(a(x))
    CASE(0)
      IF (abs(c(x) - (b_host(x) * b_host(x))) .gt. PRECISION) THEN
        errors = errors + 1
      END IF
    CASE(1)
      IF (abs(c(x) - 1) .gt. PRECISION) THEN
        errors = errors + 1
      END IF
    CASE(2)
      IF (abs(c(x)) .gt. PRECISION) THEN
        errors = errors + 1
      END IF
      IF (abs(b(x) - (b_host(x) / 2)) .gt. PRECISION) THEN
        errors = errors + 1
      END IF
    CASE DEFAULT
      IF (abs(c(x) - (b_host(x) + a(x))) .gt. PRECISION) THEN
        errors = errors + 1
      END IF
    END SELECT
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
