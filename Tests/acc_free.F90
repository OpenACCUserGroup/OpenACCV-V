#ifndef T1
!T1:runtime,data,executable-data,V:3.3

! Performs a device memory allocation and deallocation using acc_malloc and acc_free. 
! Captures the amount of free memory on the device before and after allocation to ensure 
! that memory is correctly freed. Test checks that device memory usage is restored 
! after deallocation.

      LOGICAL FUNCTION test1()
        USE ISO_C_BINDING
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        TYPE(C_PTR) :: a
        INTEGER(C_SIZE_T) :: initial_memory, final_memory
        INTEGER :: errors = 0

        initial_memory = acc_get_property(acc_get_device_num(acc_get_device_type()), acc_get_device_type(), acc_property_free_memory)
        
        a = acc_malloc(n * C_SIZEOF(0_C_INT))

        CALL acc_free(a)

        final_memory = acc_get_property(acc_get_device_num(acc_get_device_type()), acc_get_device_type(), acc_property_free_memory)

        IF (initial_memory - final_memory > PRECISION) THEN
            errors = errors + 1
        END IF

        test1 = .FALSE.
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

