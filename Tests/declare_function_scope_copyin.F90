FUNCTION function_test(a, b, c, LOOPCOUNT)
  REAL(8),DIMENSION(LOOPCOUNT),INTENT(IN) :: a, b
  REAL(8),DIMENSION(LOOPCOUNT),INTENT(INOUT) :: c
  INTEGER,INTENT(IN) :: LOOPCOUNT
  INTEGER :: x

  !$acc declare copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT))
  !$acc parallel present(c(1:LOOPCOUNT))
    !$acc loop
    DO x = 1, LOOPCOUNT
      c(x) = c(x) + a(x) + b(x)
    END DO
  !$acc end parallel
END FUNCTION function_test

FUNCTION function_test_dev_only(a, b, c, LOOPCOUNT)
  REAL(8),DIMENSION(LOOPCOUNT),INTENT(INOUT) :: a, b
  REAL(8),DIMENSION(LOOPCOUNT),INTENT(INOUT) :: c
  INTEGER,INTENT(IN) :: LOOPCOUNT
  INTEGER :: x

  !$acc declare copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT))
  !$acc parallel present(c(1:LOOPCOUNT))
    !$acc loop
    DO x = 1, LOOPCOUNT
      c(x) = c(x) + a(x) + b(x)
      a(x) = -1
      b(x) = -1
    END DO
  !$acc end parallel
END FUNCTION function_test_dev_only

#ifndef T1
!T1:devonly,construct-independent,declare,V:2.0-2.7
      LOGICAL FUNCTION test1()
  USE OPENACC
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"
  INTEGER :: errors = 0
  INTEGER :: mult = 2
  REAL(8),DIMENSION(LOOPCOUNT, LOOPCOUNT) :: a, b, c, a_host, b_host
  INTEGER,DIMENSION(1) :: devtest
  INTEGER :: x, y

  devtest(1) = 1
  !$acc enter data copyin(devtest(1:1))
  !$acc parallel present(devtest(1:1))
    devtest(1) = 0
  !$acc end parallel

  SEEDDIM(1) = 1
# ifdef SEED
  SEEDDIM(1) = SEED
# endif
  CALL RANDOM_SEED(PUT=SEEDDIM)

  CALL RANDOM_NUMBER(a)
  CALL RANDOM_NUMBER(b)
  c = 1

  !$acc data copy(c(1:LOOPCOUNT, 1:LOOPCOUNT))
    DO x = 1, LOOPCOUNT
      CALL function_test(a(:,x), b(:,x), c(:,x), LOOPCOUNT)
    END DO
  !$acc end data

  DO x = 1, LOOPCOUNT
    DO y = 1, LOOPCOUNT
      IF (abs(c(x, y) - (1 + a(x, y) + b(x, y))) .gt. PRECISION) THEN
        errors = errors + 1
        PRINT*, "1"
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
!T2:devonly,construct-independent,declare,V:2.0-2.7
      LOGICAL FUNCTION test2()
  USE OPENACC
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"
  INTEGER :: errors = 0
  INTEGER :: mult = 2
  REAL(8),DIMENSION(LOOPCOUNT, LOOPCOUNT) :: a, b, c, a_host, b_host
  INTEGER,DIMENSION(1) :: devtest
  INTEGER :: x, y

  devtest(1) = 1
  !$acc enter data copyin(devtest(1:1))
  !$acc parallel present(devtest(1:1))
    devtest(1) = 0
  !$acc end parallel

  SEEDDIM(1) = 1
# ifdef SEED
  SEEDDIM(1) = SEED
# endif
  CALL RANDOM_SEED(PUT=SEEDDIM)

  IF (devtest(1) .eq. 1) THEN
    CALL RANDOM_NUMBER(a)
    CALL RANDOM_NUMBER(b)
    c = 0
    a_host = a
    b_host = b

    !$acc data copy(a(1:LOOPCOUNT, 1:LOOPCOUNT), b(1:LOOPCOUNT, 1:LOOPCOUNT), c(1:LOOPCOUNT, 1:LOOPCOUNT))
      DO x = 1, LOOPCOUNT
        CALL function_test_dev_only(a(:,x), b(:,x), c(:,x), LOOPCOUNT)
      END DO
    !$acc end data

    DO x = 1, LOOPCOUNT
      DO y = 1, LOOPCOUNT
        IF (abs(a(x, y) + 1) .gt. PRECISION) THEN
          errors = errors + 1
          PRINT*, "2"
        END IF
        IF (abs(b(x, y) + 1) .gt. PRECISION) THEN
          errors = errors + 1
          PRINT*, "3"
        END IF
        IF (abs(c(x, y) - (a_host(x, y) + b_host(x, y))) .gt. PRECISION) THEN
          errors = errors + 1
          PRINT*, "4"
        END IF
      END DO
    END DO
  END IF

        IF (errors .eq. 0) THEN
          test2 = .FALSE.
        ELSE
          test2 = .TRUE.
        END IF
      END
#endif
#ifndef T3
!T3:devonly,construct-independent,declare,V:2.0-2.7
      LOGICAL FUNCTION test3()
  USE OPENACC
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"
  INTEGER :: errors = 0
  INTEGER :: mult = 2
  REAL(8),DIMENSION(LOOPCOUNT, LOOPCOUNT) :: a, b, c, a_host, b_host
  INTEGER,DIMENSION(1) :: devtest
  INTEGER :: x, y

  devtest(1) = 1
  !$acc enter data copyin(devtest(1:1))
  !$acc parallel present(devtest(1:1))
    devtest(1) = 0
  !$acc end parallel

  SEEDDIM(1) = 1
# ifdef SEED
  SEEDDIM(1) = SEED
# endif
  CALL RANDOM_SEED(PUT=SEEDDIM)

  IF (devtest(1) .eq. 1) THEN
    CALL RANDOM_NUMBER(a)
    CALL RANDOM_NUMBER(b)
    c = 1
    a_host = a
    b_host = b

    !$acc data copy(c(1:LOOPCOUNT, 1:LOOPCOUNt))
      DO x = 1, LOOPCOUNT
        CALL function_test_dev_only(a(:,x), b(:,x), c(:,x), LOOPCOUNT)
      END DO
    !$acc end data

    DO x = 1, LOOPCOUNT
      DO y = 1, LOOPCOUNT
        IF (abs(a(x, y) - a_host(x, y)) .gt. PRECISION) THEN
          errors = errors + 1
          PRINT*, "5"
        END IF
        IF (abs(b(x, y) - b_host(x, y)) .gt. PRECISION) THEN
          errors = errors + 1
          PRINT*, "6"
        END IF
        IF (abs(c(x, y) - (1 + a(x, y) + b(x, y))) .gt. PRECISION) THEN
          errors = errors + 1
          PRINT*, "7"
        END IF
      END DO
    END DO
  END IF

        IF (errors .eq. 0) THEN
          test3 = .FALSE.
        ELSE
          test3 = .TRUE.
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
#ifndef T3
        LOGICAL :: test3
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
#ifndef T3
        DO testrun = 1, NUM_TEST_CALLS
          failed = failed .or. test3()
        END DO
        IF (failed) THEN
          failcode = failcode + 2 ** 2
          failed = .FALSE.
        END IF
#endif
        CALL EXIT (failcode)
      END PROGRAM
