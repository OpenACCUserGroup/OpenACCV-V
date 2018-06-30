      INTEGER FUNCTION test()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.fh"
        INTEGER :: x !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, b, c, a_host, b_host !Data
        REAL(8) :: RAND
        INTEGER :: errors = 0

        !Initilization
        CALL RANDOM_SEED()
        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        c = 0
        a_host = a
        b_host = b
        WRITE(*, *) "Early"
        CALL acc_copyin_async(a(1:LOOPCOUNT), 1)
        CALL acc_copyin_async(b(1:LOOPCOUNT), 2)
        WRITE(*, *) "Here"
        !$acc data copyout(c(1:LOOPCOUNT))
          !$acc parallel async(1) present(a(1:LOOPCOUNT))
            !$acc loop
            DO x = 1, LOOPCOUNT
              a(x) = a(x) * a(x)
            END DO
          !$acc end parallel
          WRITE(*, *) "Here 2"
          !$acc parallel async(2) present(b(1:LOOPCOUNT))
            !$acc loop
            DO x = 1, LOOPCOUNT
              b(x) = b(x) * b(x)
            END DO
          !$acc end parallel
          WRITE(* ,*) "Here 3"
          !$acc parallel async(2) wait(1) present(a(1:LOOPCOUNT), b(1:LOOPCOUNT))
            !$acc loop
            DO x = 1, LOOPCOUNT
              c(x) = a(x) + b(x)
            END DO
          !$acc end parallel
        !$acc end data
        WRITE(*, *) "Here 4"
        DO x = 1, LOOPCOUNT
          IF (abs(c(x) - (a_host(x)*a_host(x) + b_host(x)*b_host(x))) .gt. PRECISION) THEN
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
