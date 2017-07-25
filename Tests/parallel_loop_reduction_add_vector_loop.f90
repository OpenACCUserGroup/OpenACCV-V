      INTEGER FUNCTION test()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.fh"
        INTEGER :: x, y !Iterators
        REAL(8),DIMENSION(10*LOOPCOUNT):: a, b !Data
        REAL(8),DIMENSION(10) :: c
        REAL(8) :: temp
        INTEGER :: errors = 0

        !Initilization
        CALL RANDOM_SEED()
        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)

        !$acc data copyin(a(1:10*LOOPCOUNT), b(1:10*LOOPCOUNT)) !$acc &
            !$acc copyout(c(1:10))
          !$acc parallel loop private(temp)
          DO x = 0, 9
            temp = 0.0
            !$acc loop vector reduction(+:temp)
            DO y = 1, LOOPCOUNT
              temp = temp + a(x * LOOPCOUNT + y) + b(x * LOOPCOUNT + y)
            END DO
            c(x + 1) = temp
          END DO
        !$acc end data

        DO x = 0, 9
          temp = 0
          DO y = 1, LOOPCOUNT
            temp = temp + a(x * LOOPCOUNT + y) + b(x * LOOPCOUNT + y)
          END DO
          IF (abs(temp - c(x + 1)) .gt. PRECISION) THEN
            errors = errors + 1
          END IF
        END DO
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
      WRITE (*,*) "Test of parallel_loop_reduction_add_vector_loop"
      WRITE (*,*) "--------------------------------------------------"

      crossfailed=0
      result=1
      WRITE (1,*) "--------------------------------------------------"
      WRITE (1,*) "Test of parallel_loop_reduction_add_vector_loop"
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


