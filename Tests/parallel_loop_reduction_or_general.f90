      INTEGER FUNCTION test()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.f90"
        INTEGER :: x !Iterators
        LOGICAL,DIMENSION(LOOPCOUNT):: a !Data
        LOGICAL :: results = .FALSE.
        LOGICAL :: temp = .FALSE.
        REAL(8) :: false_margin = exp(log(.5) / n)
        REAL(8),DIMENSION(LOOPCOUNT):: random
        INTEGER :: errors = 0

        !Initilization
        CALL RANDOM_SEED()
        CALL RANDOM_NUMBER(random)
        DO x = 1, LOOPCOUNT
          IF (random(x) > false_margin) THEN
            a(x) = .TRUE.
          ELSE
            a(x) = .FALSE.
          END IF
        END DO

        !$acc data copyin(a(1:LOOPCOUNT))
          !$acc parallel loop reduction(.OR.:results)
          DO x = 1, LOOPCOUNT
            results = results .OR. a(x)
          END DO
        !$acc end data

        DO x = 1, LOOPCOUNT
          temp = temp .OR. a(x)
        END DO
        IF (temp .neqv. results) THEN
          errors = errors + 1
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
      WRITE (*,*) "Test of parallel_loop_reduction_or_general"
      WRITE (*,*) "--------------------------------------------------"

      crossfailed=0
      result=1
      WRITE (1,*) "--------------------------------------------------"
      WRITE (1,*) "Test of parallel_loop_reduction_or_general"
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
                                             
