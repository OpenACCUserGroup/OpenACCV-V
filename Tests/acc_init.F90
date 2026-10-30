#ifndef T1
!T1:runtime,construct-independent,internal-control-values,init,nonvalidating,V:1.0-2.7
      LOGICAL FUNCTION test1()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        IF (acc_get_device_type() .ne. acc_device_none) THEN
          CALL acc_init(acc_get_device_type())
        END IF

        test1 = .FALSE.
      END
#endif

#ifndef T2
!T2:runtime,init,parallel,V:2.5-2.7
      LOGICAL FUNCTION test2()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        REAL(8), DIMENSION(LOOPCOUNT) :: a
        INTEGER :: i, errors
        
        errors = 0
        DO i = 1, LOOPCOUNT
          a(i) = REAL(i - 1, 8)
        END DO

        IF (acc_get_device_type() .ne. acc_device_none) THEN
          CALL acc_init(acc_get_device_type())
        END IF

        !$acc parallel loop copy(a)
        DO i = 1, LOOPCOUNT
          a(i) = a(i) * 2.0d0
        END DO

        DO i = 1, LOOPCOUNT
          IF (ABS(a(i) - (2.0d0 * (i - 1))) > PRECISION) THEN
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
        !Conditionally define test functions
#ifndef T1
        LOGICAL :: test1
#endif
#ifndef T2
        LOGICAL :: test2
#endif
        failcode = 0
        failed = .FALSE.

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