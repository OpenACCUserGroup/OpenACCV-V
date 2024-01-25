#ifndef T1
!T1:runtime,construct-independent,internal-control-values,init,nonvalidating,V:3.3
      LOGICAL FUNCTION test1()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        REAL(8),DIMENSION(LOOPCOUNT):: initial_memory, final_memory !Data
        INTEGER, POINTER :: a(:)
        INTEGER :: errors = 0

        initial_memory = acc_get_property(acc_get_device_num(acc_get_device_type()), acc_get_device_type(), acc_property_free_memory)
        
        CALL acc_malloc(a(N))

        IF (initial_memory .ne. 0) THEN
          test1 = .FALSE.
        END IF

        final_memory = acc_get_property(acc_get_device_num(acc_get_device_type()), acc_get_device_type(), acc_property_free_memory)

        DO x = 1, LOOPCOUNT
          IF (final_memory + N * sizeof(a(1)) .gt. initial_memory) THEN
            errors = errors + 1
          END IF
        END DO

        CALL acc_free(a(N))

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
        !Conditionally define test functions
#ifndef T1
        LOGICAL :: test1
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
        CALL EXIT (failcode)
      END PROGRAM


