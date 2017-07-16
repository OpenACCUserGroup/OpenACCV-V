      INTEGER FUNCTION test()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.f90"
        INTEGER :: x !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, b !Data
        INTEGER :: errors = 0
        LOGICAL,DIMENSION(1):: devtest
        LOGICAL:: data_on_device = .FALSE.
        devtest(1) = .TRUE.

        !$acc enter data copyin(devtest(1:1))
        !$acc kernels present(devtest(1:1))
          devtest(1) = .FALSE.
        !$acc end kernels

        !Initilization
        CALL RANDOM_SEED()
        CALL RANDOM_NUMBER(a)
        b = 0

        !$acc kernels if(data_on_device) present(a(1:LOOPCOUNT), b(1:LOOPCOUNT))
          !$acc loop
          DO x = 1, LOOPCOUNT
            b(x) = a(x)
          END DO
        !$acc end kernels

        DO x = 1, LOOPCOUNT
          IF (abs(a(x) - b(x)) .gt. PRECISION) THEN
            errors = errors + 1
          END IF
        END DO

        CALL RANDOM_NUMBER(a)
        b = 0

        data_on_device = .TRUE.
        !$acc enter data copyin(a(1:LOOPCOUNT)) create(b(1:LOOPCOUNT))
        !$acc kernels if(data_on_device) present(a(1:LOOPCOUNT), b(1:LOOPCOUNT))
          !$acc loop
          DO x = 1, LOOPCOUNT
            b(x) = a(x)
          END DO
        !$acc end kernels
        !$acc exit data copyout(b(1:LOOPCOUNT)) delete(a(1:LOOPCOUNT))
        
        DO x = 1, LOOPCOUNT
          IF (abs(b(x) - a(x)) .gt. PRECISION) THEN
            errors = errors + 1
          END IF
        END DO

        IF (devtest(1) .eqv. .TRUE.) THEN
          CALL RANDOM_NUMBER(a)
          b = 0
 
          !$acc enter data copyin(a(1:LOOPCOUNT)) create(b(1:LOOPCOUNT))
          DO x = 1, LOOPCOUNT
            a(x) = -1
          END DO
          
          !$acc kernels if(data_on_device) present(a(1:LOOPCOUNT), b(1:LOOPCOUNT))
            !$acc loop
            DO x = 1, LOOPCOUNT
              b(x) = a(x)
            END DO
          !$acc end kernels
 
          DO x = 1, LOOPCOUNT
            IF (abs(a(x) + 1) .gt. PRECISION) THEN
              errors = errors + 1
            END IF
            IF (abs(b(x)) .gt. PRECISION) THEN
              errors = errors + 1
            END IF
          END DO
 
          !$acc exit data copyout(a(1:LOOPCOUNT), b(1:LOOPCOUNT))
          data_on_device = .FALSE.

          DO x = 1, LOOPCOUNT
            IF (abs(a(x) - b(x)) .gt. PRECISION) THEN
              errors = errors + 1
            END IF
          END DO

          CALL RANDOM_NUMBER(a)
          b = 0

          !$acc enter data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT))
          !$acc kernels if(data_on_device) present(a(1:LOOPCOUNT), b(1:LOOPCOUNT))
            !$acc loop
            DO x = 1, LOOPCOUNT
              b(x) = a(x)
            END DO
          !$acc end kernels

          DO x = 1, LOOPCOUNT
            IF (abs(a(x) - b(x)) .gt. PRECISION) THEN
              errors = errors + 1
            END IF
          END DO

          !$acc exit data copyout(a(1:LOOPCOUNT), b(1:LOOPCOUNT))
          
          DO x = 1, LOOPCOUNT
            IF (abs(b(x)) .gt. PRECISION) THEN
              IF (abs(b(x) - a(x)) .gt. PRECISION) THEN
                errors = errors + 1
              END IF
            END IF
          END DO
        END IF

        test = errors
      END


      PROGRAM test_kernels_async_main
      IMPLICIT NONE
      INTEGER :: failed, success !Number of failed/succeeded tests
      INTEGER :: num_tests,crosschecked, crossfailed, j
      INTEGER :: temp,temp1
      INCLUDE "acc_testsuite.f90"
      INTEGER test


      CHARACTER*50:: logfilename !Pointer to logfile
      INTEGER :: result

      num_tests = 0
      crosschecked = 0
      crossfailed = 0
      result = 1
      failed = 0

      !Open a new logfile or overwrite the existing one.
      logfilename = "OpenACC_testsuite.log"
!      WRITE (*,*) "Enter logFilename:"
!      READ  (*,*) logfilename

      OPEN (1, FILE = logfilename)

      WRITE (*,*) "######## OpenACC Validation Suite V 2.5 ######"
      WRITE (*,*) "## Repetitions:", N
      WRITE (*,*) "## Loop Count :", LOOPCOUNT
      WRITE (*,*) "##############################################"
      WRITE (*,*)

      WRITE (*,*) "--------------------------------------------------"
      WRITE (*,*) "Test of kernels_if"
      WRITE (*,*) "--------------------------------------------------"

      crossfailed=0
      result=1
      WRITE (1,*) "--------------------------------------------------"
      WRITE (1,*) "Test of kernels_if"
      WRITE (1,*) "--------------------------------------------------"
      WRITE (1,*)
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
                                             

