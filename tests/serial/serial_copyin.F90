#ifndef T1
!T1:serial,V:2.6-2.7
      LOGICAL FUNCTION test1()
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"
  INTEGER:: errors
  REAL(8),DIMENSION(LOOPCOUNT):: a, a_copy, b
  INTEGER,DIMENSION(1):: hasDevice
  INTEGER:: x
  errors = 0

  SEEDDIM(1) = 1
# ifdef SEED
  SEEDDIM(1) = SEED
# endif
  CALL RANDOM_SEED(PUT=SEEDDIM)

  CALL RANDOM_NUMBER(a)
  a_copy = a
  b = 0

  hasDevice(1) = 1
  !$acc enter data copyin(hasDevice(1:1))
  !$acc parallel present(hasDevice(1:1))
    hasDevice(1) = 0
  !$acc end parallel

  !$acc serial copyin(a(1:LOOPCOUNT))
    !$acc loop
    DO x = 1, LOOPCOUNT
      a(x) = 0
    END DO
  !$acc end serial

  DO x = 1, LOOPCOUNT
    IF (hasDevice(1) .eq. 1) THEN
      IF (abs(a(x) - a_copy(x)) .gt. PRECISION) THEN
        errors = errors + 1
      END IF
    ELSE
      IF (abs(a(x)) .gt. PRECISION) THEN
        errors = errors + 1
      END IF
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
!T2:serial,V:2.6-2.7
      LOGICAL FUNCTION test2()
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"
  INTEGER:: errors
  REAL(8),DIMENSION(LOOPCOUNT):: a, a_copy, b
  INTEGER,DIMENSION(1):: hasDevice
  INTEGER:: x
  errors = 0

  SEEDDIM(1) = 1
# ifdef SEED
  SEEDDIM(1) = SEED
# endif
  CALL RANDOM_SEED(PUT=SEEDDIM)

  CALL RANDOM_NUMBER(a)

  !$acc data copy(b(1:LOOPCOUNT))
    !$acc serial copyin(a(1:LOOPCOUNT))
      !$acc loop
      DO x = 1, LOOPCOUNT
        b(x) = a(x)
      END DO
    !$acc end serial
  !$acc end data

  DO x = 1, LOOPCOUNT
    IF (abs(a(x) - b(x)) .gt. PRECISION) THEN
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
