#ifndef T1
!T1:devonly,serial,if,V:2.6-2.7
      LOGICAL FUNCTION test1()
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"
  INTEGER:: errors
  REAL(8),DIMENSION(LOOPCOUNT):: a, b, c
  LOGICAL,DIMENSION(1):: devtest
  LOGICAL:: host, device
  INTEGER:: x
  host = .FALSE.
  device = .TRUE.
  errors = 0

  devtest(1) = .TRUE.
  !$acc enter data copyin(devtest(1:1))
  !$acc parallel present(devtest(1:1))
    devtest(1) = .FALSE.
  !$acc end parallel

  SEEDDIM(1) = 1
# ifdef SEED
  SEEDDIM(1) = SEED
# endif
  CALL RANDOM_SEED(PUT=SEEDDIM)

  CALL RANDOM_NUMBER(a)
  CALL RANDOM_NUMBER(b)
  c = 0

  !$acc serial if(host)
    !$acc loop
    DO x = 1, LOOPCOUNT
      c(x) = a(x) + b(x)
    END DO
  !$acc end serial

  DO x = 1, N
    IF (abs(c(x) - (a(x) + b(x))) .gt. PRECISION) THEN
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
!T2:devonly,serial,if,V:2.6-2.7
      LOGICAL FUNCTION test2()
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"
  INTEGER:: errors
  REAL(8),DIMENSION(LOOPCOUNT):: a, b, c
  LOGICAL,DIMENSION(1):: devtest
  LOGICAL:: host, device
  INTEGER:: x
  host = .FALSE.
  device = .TRUE.
  errors = 0

  devtest(1) = .TRUE.
  !$acc enter data copyin(devtest(1:1))
  !$acc parallel present(devtest(1:1))
    devtest(1) = .FALSE.
  !$acc end parallel

  SEEDDIM(1) = 1
# ifdef SEED
  SEEDDIM(1) = SEED
# endif
  CALL RANDOM_SEED(PUT=SEEDDIM)

  IF (devtest(1)) THEN
    CALL RANDOM_NUMBER(a)
    CALL RANDOM_NUMBER(b)
    c = a + b

    !$acc enter data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT), c(1:LOOPCOUNT))
    !$acc serial if(host) present(a(1:LOOPCOUNT), b(1:LOOPCOUNT), c(1:LOOPCOUNT))
      !$acc loop
      DO x = 1, LOOPCOUNT
        c(x) = c(x) + a(x) + b(x)
      END DO
    !$acc end serial
    !$acc exit data delete(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) copyout(c(1:LOOPCOUNT))

    DO x = 1, LOOPCOUNT
      IF (abs(c(x) - (a(x) + b(x))) .gt. PRECISION) THEN
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
!T3:devonly,serial,if,V:2.6-2.7
      LOGICAL FUNCTION test3()
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"
  INTEGER:: errors
  REAL(8),DIMENSION(LOOPCOUNT):: a, b, c
  LOGICAL,DIMENSION(1):: devtest
  LOGICAL:: host, device
  INTEGER:: x
  host = .FALSE.
  device = .TRUE.
  errors = 0

  devtest(1) = .TRUE.
  !$acc enter data copyin(devtest(1:1))
  !$acc parallel present(devtest(1:1))
    devtest(1) = .FALSE.
  !$acc end parallel

  SEEDDIM(1) = 1
# ifdef SEED
  SEEDDIM(1) = SEED
# endif
  CALL RANDOM_SEED(PUT=SEEDDIM)

  CALL RANDOM_NUMBER(a)
  CALL RANDOM_NUMBER(b)
  c = a + b
  
  !$acc enter data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT), c(1:LOOPCOUNT))
  !$acc serial if(device) present(a(1:LOOPCOUNT), b(1:LOOPCOUNT), c(1:LOOPCOUNT))
    !$acc loop
    DO x = 1, LOOPCOUNT
      c(x) = c(x) + a(x) + b(x)
    END DO
  !$acc end serial
  !$acc exit data delete(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) copyout(c(1:LOOPCOUNT))

  DO x = 1, LOOPCOUNT
    IF (abs(c(x) - (2 * (a(x) + b(x)))) .gt. PRECISION * 2) THEN
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
