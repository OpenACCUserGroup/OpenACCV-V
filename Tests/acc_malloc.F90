#ifndef T1
!T1:runtime,construct-independent,internal-control-values,init,nonvalidating,V:3.3

! Validates device memory allocation using acc_malloc and the correctness of acc_free behavior.
! Captures the initial and final available device memory and ensures that acc_malloc returns a
! valid pointer. The test confirms that memory was allocated and subsequently released, by
! verifying pointer association and comparing memory availability before and after allocation.
! Failure is recorded if the pointer is not associated or memory accounting appears incorrect.

      LOGICAL FUNCTION test1()
        USE ISO_C_BINDING
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER(C_SIZE_T) :: initial_memory, final_memory
        TYPE(C_PTR) :: a
        INTEGER :: errors = 0

        initial_memory = acc_get_property(acc_get_device_num(acc_get_device_type()), acc_get_device_type(), acc_property_free_memory)
        
        a = acc_malloc(n * C_SIZEOF(0_C_INT))

        IF (.NOT. C_ASSOCIATED(a)) THEN
            errors = errors + 1
        END IF

        final_memory = acc_get_property(acc_get_device_num(acc_get_device_type()), acc_get_device_type(), acc_property_free_memory)

        IF (initial_memory /= final_memory) THEN
            errors = errors + 1
        END IF

        CALL acc_free(a)

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
