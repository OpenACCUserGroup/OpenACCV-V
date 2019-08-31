      INTEGER FUNCTION test()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.fh"
        INTEGER :: x, y !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a !Data
        REAL(8),DIMENSION(:, :),ALLOCATABLE :: host_copy
        REAL(8) :: RAND
        INTEGER :: errors = 0

        CALL RANDOM_SEED
        ALLOCATE(host_copy(acc_get_num_devices(acc_get_device_type()), LOOPCOUNT))

        IF (acc_get_device_type() .ne. acc_device_none) THEN
          !host_copy(acc_get_num_devices(acc_get_device_type()), LOOPCOUNT)
          DO x = 1, acc_get_num_devices(acc_get_device_type())
            CALL RANDOM_NUMBER(a)
            host_copy(x, :) = a
            CALL acc_set_device_num(x, acc_get_device_type())
            !$acc enter data copyin(a(1:LOOPCOUNT))
          END DO

          DO x = 1, acc_get_num_devices(acc_get_device_type)
            CALL acc_set_device_num(x, acc_get_device_type())
            !$acc data present(a(1:LOOPCOUNT))
              !$acc parallel
                !$acc loop
                DO y = 1, LOOPCOUNT
                  a(y) = a(y) + 1
                END DO
              !$acc end parallel
            !$acc end data
          END DO

          DO x = 1, acc_get_num_devices(acc_get_device_type())
            CALL acc_set_device_num(x, acc_get_device_type())
            !$acc exit data copyout(a(1:LOOPCOUNT))
            DO y = 1, LOOPCOUNT
              IF (abs(a(y) - (host_copy(x, y) + 1)) .gt. PRECISION) THEN
                errors = errors + 1
              END IF
            END DO
          END DO
        END IF

        test = errors
      END


      PROGRAM test_kernels_async_main
      IMPLICIT NONE
      INTEGER :: failed, success !Number of failed/succeeded tests
      INTEGER :: num_tests,crosschecked, crossfailed, j
      INTEGER :: temp,temp1
      INCLUDE "acc_testsuite.fh"
      INTEGER test


      CHARACTER*50:: logfilename !Pointer to logfile
      INTEGER :: result

      num_tests = 0
      crosschecked = 0
      crossfailed = 0
      result = 1
      failed = 0

      !Open a new logfile or overwrite the existing one.
      logfilename = "test.log"
!      WRITE (*,*) "Enter logFilename:"
!      READ  (*,*) logfilename

      OPEN (1, FILE = logfilename)

      WRITE (*,*) "######## OpenACC Validation Suite V 1.0a ######"
      WRITE (*,*) "## Repetitions:", N
      WRITE (*,*) "## Loop Count :", LOOPCOUNT
      WRITE (*,*) "##############################################"
      WRITE (*,*)

      WRITE (*,*) "--------------------------------------------------"
      !WRITE (*,*) "Testing acc_kernels_async"
      WRITE (*,*) "Testing test_kernels_async"
      WRITE (*,*) "--------------------------------------------------"

      crossfailed=0
      result=1
      WRITE (1,*) "--------------------------------------------------"
      !WRITE (1,*) "Testing acc_kernels_async"
      WRITE (1,*) "Testing test_kernels_async"
      WRITE (1,*) "--------------------------------------------------"
      WRITE (1,*)
      WRITE (1,*) "testname: test_kernels_async"
      WRITE (1,*) "(Crosstests should fail)"
      WRITE (1,*)

      DO j = 1, N
        temp =  test()
        IF (temp .EQ. 0) THEN
          WRITE (1,*)  j, ". test successfull."
          success = success + 1
        ELSE
          WRITE (1,*) "Error: ",j, ". test failed."
          failed = failed + 1
        ENDIF
      END DO


      IF (failed .EQ. 0) THEN
        WRITE (1,*) "Directive worked without errors."
        WRITE (*,*) "Directive worked without errors."
        result = 0
        WRITE (*,*) "Result:",result
      ELSE
        WRITE (1,*) "Directive failed the test ", failed, " times."
        WRITE (*,*) "Directive failed the test ", failed, " times."
        result = failed * 100 / N
        WRITE (*,*) "Result:",result
      ENDIF
      CALL EXIT (result)
      END PROGRAM
