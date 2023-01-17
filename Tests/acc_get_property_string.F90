#ifndef T1
!T1:runtime,construct-independent,internal-control-values,nonvalidating,V:2.6-2.7
      LOGICAL FUNCTION test1()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        CHARACTER(len=:),ALLOCATABLE :: output1
        CHARACTER(len=:),ALLOCATABLE :: output2
        CHARACTER(len=:),ALLOCATABLE :: output3
        IF (acc_get_device_type() .ne. acc_device_none) THEN
          ! Hopefully, the string is long enough:
          allocate(character(len=1024) :: output1, output2, output3)
          CALL acc_get_property_string(acc_get_device_num(acc_get_device_type()), acc_get_device_type(), &
                                                          acc_property_name, output1)
          CALL acc_get_property_string(acc_get_device_num(acc_get_device_type()), acc_get_device_type(), &
                                                          acc_property_vendor, output2)
          CALL acc_get_property_string(acc_get_device_num(acc_get_device_type()), acc_get_device_type(), &
                                                          acc_property_driver, output3)
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

