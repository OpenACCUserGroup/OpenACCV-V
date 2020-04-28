#ifndef T1
!T1:serial,firstprivate,V:2.6-2.7
      LOGICAL FUNCTION test1()
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"
  INTEGER:: errors
  REAL(8),DIMENSION(10, LOOPCOUNT):: a, b, d
  REAL(8),DIMENSION(10):: c, c_copy
  INTEGER:: x, y

  errors = 0

  SEEDDIM(1) = 1
# ifdef SEED
  SEEDDIM(1) = SEED
# endif
  CALL RANDOM_SEED(PUT=SEEDDIM)

  CALL RANDOM_NUMBER(a)
  CALL RANDOM_NUMBER(b)
  CALL RANDOM_NUMBER(c)
  c_copy = c
  d = 0
  !$acc data copyin(a(1:10, 1:LOOPCOUNT), b(1:10, 1:LOOPCOUNT)) copy(d(1:10, 1:LOOPCOUNT))
    !$acc serial firstprivate(c(1:10))
      !$acc loop gang
      DO y = 1, LOOPCOUNT
        !$acc loop worker
        DO x = 1, 10
          d(x, y) = a(x, y) + b(x, y) + c(x)
        END DO
      END DO
    !$acc end serial
  !$acc end data

  DO y = 1, LOOPCOUNT
    DO x = 1, 10
      IF (abs(d(x, y) - (a(x, y) + b(x, y) + c_copy(x))) .gt. PRECISION) THEN
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
#ifndef T2
!T2:serial,firstprivate,V:2.6-2.7
      LOGICAL FUNCTION test2()
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"
  INTEGER:: errors
  REAL(8),DIMENSION(10, LOOPCOUNT):: a, b, d
  REAL(8),DIMENSION(10):: c, c_copy
  INTEGER:: x, y

  errors = 0

  SEEDDIM(1) = 1
# ifdef SEED
  SEEDDIM(1) = SEED
# endif
  CALL RANDOM_SEED(PUT=SEEDDIM)

  CALL RANDOM_NUMBER(a)
  CALL RANDOM_NUMBER(b)
  d = 0
  c = 0

  !$acc data copyin(a(1:10, 1:LOOPCOUNT), b(1:10, 1:LOOPCOUNT)) copy(d(1:10, 1:LOOPCOUNT))
    !$acc serial firstprivate(c(1:10))
      !$acc loop gang independent
      DO y = 1, LOOPCOUNT
        !$acc loop worker independent
        DO x = 1, 10
          c(x) = a(x, y) - b(x, y)
        END DO
        !$acc loop worker independent
        DO x = 1, 10
          d(x, y) = a(x, y) + b(x, y) + c(x)
        END DO
      END DO
    !$acc end serial
  !$acc end data

  DO y = 1, LOOPCOUNT
    DO x = 1, 10
      IF (abs(d(x, y) - (2 * a(x, y))) .gt. PRECISION) THEN
        errors = errors + 1
      END IF
    END DO
  END DO

        IF (errors .eq. 0) THEN
          test2 = .FALSE.
        ELSE
          test2 = .TRUE.
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
#ifndef T2
        LOGICAL :: test2
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
#ifndef T2
        DO testrun = 1, NUM_TEST_CALLS
          failed = failed .or. test2()
        END DO
        IF (failed) THEN
          failcode = failcode + 2 ** 1
          failed = .FALSE.
        END IF
#endif
        CALL EXIT (failcode)
      END PROGRAM
