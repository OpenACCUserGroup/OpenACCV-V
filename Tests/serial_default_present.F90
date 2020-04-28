#ifndef T1
!T1:serial,default,V:2.6-2.7
      LOGICAL FUNCTION test1()
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"
  INTEGER:: errors
  REAL(8),DIMENSION(LOOPCOUNT):: a
  INTEGER:: x
  errors = 0

  a = 0

  !$acc enter data copyin(a(1:LOOPCOUNT))
  !$acc serial default(present)
    !$acc loop
    DO x = 1, LOOPCOUNT
      a(x) = 1
    END DO
  !$acc end serial
  !$acc exit data copyout(a(1:LOOPCOUNT))

  DO x = 1, LOOPCOUNT
    IF (abs(a(x) - 1) .gt. PRECISION) THEN
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
