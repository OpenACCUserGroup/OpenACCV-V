#ifndef T1
!T1:serial,V:2.6-2.7
      LOGICAL FUNCTION test1()
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"
  INTEGER:: errors
  REAL(8),DIMENSION(LOOPCOUNT):: a, b
  INTEGER:: x
  INTEGER,DIMENSION(1):: hasDevice
  errors = 0

  hasDevice(1) = 1
  !$acc enter data copyin(hasDevice(1:1))
  !$acc parallel present(hasDevice(1:1))
  hasDevice(1) = 0
  !$acc end parallel
  
  SEEDDIM(1) = 1
# ifdef SEED
  SEEDDIM(1) = SEED
# endif
  CALL RANDOM_SEED(PUT=SEEDDIM)

  CALL RANDOM_NUMBER(a)
  b = 0


  !$acc data copyin(a(1:LOOPCOUNT))
    !$acc serial copyout(b(1:LOOPCOUNT))
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
  REAL(8),DIMENSION(LOOPCOUNT):: a, b
  INTEGER:: x
  INTEGER,DIMENSION(1):: hasDevice
  errors = 0

  hasDevice(1) = 1
  !$acc enter data copyin(hasDevice(1:1))
  !$acc parallel present(hasDevice(1:1))
  hasDevice(1) = 0
  !$acc end parallel
  
  SEEDDIM(1) = 1
# ifdef SEED
  SEEDDIM(1) = SEED
# endif
  CALL RANDOM_SEED(PUT=SEEDDIM)

  IF (hasDevice(1) .eq. 1) THEN
    !$acc data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT))
      !$acc serial copyout(b(1:LOOPCOUNT))
        !$acc loop
        DO x = 1, LOOPCOUNT
          b(x) = b(x) - a(x)
        END DO
      !$acc end serial
    !$acc end data

    DO x = 1, LOOPCOUNT
      IF (abs(a(x) - b(x)) .gt. PRECISION) THEN
        errors = errors + 1
      END IF
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
!T3:serial,update,V:2.6-2.7
      LOGICAL FUNCTION test3()
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"
  INTEGER:: errors
  REAL(8),DIMENSION(LOOPCOUNT):: a, b
  INTEGER:: x
  INTEGER,DIMENSION(1):: hasDevice
  errors = 0

  hasDevice(1) = 1
  !$acc enter data copyin(hasDevice(1:1))
  !$acc parallel present(hasDevice(1:1))
  hasDevice(1) = 0
  !$acc end parallel
  
  SEEDDIM(1) = 1
# ifdef SEED
  SEEDDIM(1) = SEED
# endif
  CALL RANDOM_SEED(PUT=SEEDDIM)

  !$acc data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT))
    !$acc serial copyout(b(1:LOOPCOUNT))
      !$acc loop
      DO x = 1, LOOPCOUNT
        b(x) = b(x) - a(x)
      END DO
    !$acc end serial
    !$acc update host(b(1:LOOPCOUNT))
  !$acc end data

  DO x = 1, LOOPCOUNT
    IF (abs(b(x)) .gt. 2 * PRECISION) THEN
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
