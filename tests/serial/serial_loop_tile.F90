#ifndef T1
!T1:serial,combined-constructs,loop,tile,V:2.6-2.7
      LOGICAL FUNCTION test1()
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"
  INTEGER:: errors
  REAL(8),DIMENSION(100):: a, b, c
  REAL(8),DIMENSION(100, 100):: d2
  REAL(8),DIMENSION(100, 100, 100):: d3
  INTEGER:: x, y, z
  errors = 0

  SEEDDIM(1) = 1
# ifdef SEED
  SEEDDIM(1) = SEED
# endif
  CALL RANDOM_SEED(PUT=SEEDDIM)

  CALL RANDOM_NUMBER(a)
  CALL RANDOM_NUMBER(b)
  CALL RANDOM_NUMBER(c)
  d2 = 0
  d3 = 0

  !$acc data copyin(a(1:100), b(1:100), c(1:100)) copyout(d2(1:100, 1:100))
    !$acc serial loop tile(*, *)
    DO x = 1, 100
      DO y = 1, 100
        DO z = 1, 100
          d2(x, y) = d2(x, y) + (a(x) * b(y) * c(z))
        END DO
      END DO
    END DO
  !$acc end data

  DO x = 1, 100
    DO y = 1, 100
      DO z = 1, 100
        d2(x, y) = d2(x, y) - (a(x) * b(y) * c(z))
      END DO
      IF (abs(d2(x, y)) .gt. 10*PRECISION) THEN
        errors = errors + 1
      END IF
    END DO
  END DO

  !$acc data copyin(a(1:100), b(1:100), c(1:100)) copyout(d3(1:100, 1:100, 1:100))
    !$acc serial loop tile(*, *, *)
    DO x = 1, 100
      DO y = 1, 100
        DO z = 1, 100
          d3(x, y, z) = a(x) * b(y) * c(z)
        END DO
      END DO
    END DO
  !$acc end data

  DO x = 1, 100
    DO y = 1, 100
      DO z = 1, 100
        IF (abs(d3(x, y, z) - (a(x) * b(y) * c(z))) .gt. PRECISION) THEN
          errors = errors + 1
        END IF
      END DO
    END DO
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
