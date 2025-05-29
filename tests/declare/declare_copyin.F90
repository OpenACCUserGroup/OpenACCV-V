!$acc routine vector
FUNCTION multiplyData(a)
  REAL(8),DIMENSION(LOOPCOUNT), INTENT(INOUT) :: a
  INTEGER :: x
  !$acc loop vector
  DO x = 1, LOOPCOUNT
    a(x) = a(x) * 2
  END DO
END FUNCTION multiplyData

#ifndef T1
!T1:construct-independent,declare,V:2.0-2.7
      LOGICAL FUNCTION test1()
  USE OPENACC
  USE DECLARE_COPYIN_MOD
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"
  INTEGER :: errors = 0
  INTEGER :: mult = 2
  INTEGER :: x
  REAL(8),DIMENSION(LOOPCOUNT) :: a, b

  SEEDDIM(1) = 1
# ifdef SEED
  SEEDDIM(1) = SEED
# endif
  CALL RANDOM_SEED(PUT=SEEDDIM)

  CALL RANDOM_NUMBER(a)
  b = 0

  !$acc data copyin(a(1:LOOPCOUNT)) copyout(b(1:LOOPCOUNT)) present(fixed_size_array)
    !$acc parallel
      !$acc loop
      DO x = 1, LOOPCOUNT
        b(x) = a(x) + fixed_size_array(MOD(x, 10))
      END DO
    !$acc end parallel
  !$acc end data

  DO x = 1, LOOPCOUNT
    IF (abs(b(x) - (a(x) + fixed_size_array(MOD(x, 10)))) .gt. PRECISION) THEN
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
#ifndef T2
!T2:construct-independent,declare,V:1.0-2.7
      LOGICAL FUNCTION test2()
  USE OPENACC
  USE DECLARE_COPYIN_MOD
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"
  INTEGER :: errors = 0
  INTEGER :: mult = 2
  REAL(8),DIMENSION(LOOPCOUNT) :: a, b

  SEEDDIM(1) = 1
# ifdef SEED
  SEEDDIM(1) = SEED
# endif
  CALL RANDOM_SEED(PUT=SEEDDIM)

  CALL RANDOM_NUMBER(a)
  b = 0

  !$acc data copyin(a(1:LOOPCOUNT)) copyout(b(1:LOOPCOUNT)) present(scalar)
    !$acc parallel
      !$acc loop
      DO x = 1, LOOPCOUNT
        b(x) = a(x) + scalar
      END DO
    !$acc end parallel
  !$acc end data

  DO x = 1, LOOPCOUNT
    IF (abs(b(x) - (a(x) + scalar)) .gt. PRECISION) THEN
      errors = errors + 1
    END IF
  END DO

        IF (errors .eq. 0) THEN
          test2 = .FALSE.
        ELSE
          test2 = .TRUE.
        END IF
      END
#endif
#ifndef T3
!T3:construct-independent,declare,V:1.0-2.7
      LOGICAL FUNCTION test3()
  USE OPENACC
  USE DECLARE_COPYIN_MOD
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"
  INTEGER :: errors = 0
  INTEGER :: mult = 2
  REAL(8),DIMENSION(LOOPCOUNT) :: a, b

  SEEDDIM(1) = 1
# ifdef SEED
  SEEDDIM(1) = SEED
# endif
  CALL RANDOM_SEED(PUT=SEEDDIM)

  CALL RANDOM_NUMBER(a)
  b = a

  !$acc data copy(a(1:LOOPCOUNT))
    !$acc parallel
      !$acc loop
      DO x = 1, 1
        CALL externMultiplyData(a, LOOPCOUNT)
      END DO
    !$acc end parallel
  !$acc end data

  DO x = 1, LOOPCOUNT
    IF (abs(a(x) - (b(x) * 2)) .gt. PRECISION) THEN
      errors = errors + 1
    END IF
  END DO

        IF (errors .eq. 0) THEN
          test3 = .FALSE.
        ELSE
          test3 = .TRUE.
        END IF
      END
#endif
#ifndef T4
!T4:construct-independent,declare,V:1.0-2.7
      LOGICAL FUNCTION test4()
  USE OPENACC
  USE DECLARE_COPYIN_MOD
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"
  INTEGER :: errors = 0
  INTEGER :: mult = 2
  REAL(8),DIMENSION(LOOPCOUNT) :: a, b

  SEEDDIM(1) = 1
# ifdef SEED
  SEEDDIM(1) = SEED
# endif
  CALL RANDOM_SEED(PUT=SEEDDIM)

  CALL RANDOM_NUMBER(a)
  b = a

  !$acc data copy(a(1:LOOPCOUNT))
    !$acc parallel
      !$acc loop
      DO x = 1, 1
        CALL multiplyData(a)
      END DO
    !$acc end parallel
  !$acc end data

  DO x = 1, LOOPCOUNT
    IF (abs(a(x) - (b(x) * 2)) .gt. PRECISION) THEN
      errors = errors + 1
    END IF
  END DO

        IF (errors .eq. 0) THEN
          test4 = .FALSE.
        ELSE
          test4 = .TRUE.
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
#ifndef T4
        LOGICAL :: test4
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
#ifndef T4
        DO testrun = 1, NUM_TEST_CALLS
          failed = failed .or. test4()
        END DO
        IF (failed) THEN
          failcode = failcode + 2 ** 3
          failed = .FALSE.
        END IF
#endif
        CALL EXIT (failcode)
      END PROGRAM
