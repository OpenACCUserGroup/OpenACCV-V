      INTEGER FUNCTION test()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.f90"
        INTEGER :: x, y !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, b, c, d, e !Data
        REAL(8) :: RAND
        INTEGER :: errors = 0

        !Initilization
        CALL RANDOM_SEED()
        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        CALL RANDOM_NUMBER(d)
        c = 0
        e = 0

        !$acc enter data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) create(c(1:LOOPCOUNT)) async(1)
        !$acc enter data copyin(d(1:LOOPCOUNT)) create(e(1:LOOPCOUNT)) async(2)
        !$acc parallel present(a(1:LOOPCOUNT), b(1:LOOPCOUNT), c(1:LOOPCOUNT)) async(1)
          !$acc loop
          DO x = 1, LOOPCOUNT
            c(x) = a(x) + b(x)
          END DO
        !$acc end parallel
        !$acc parallel present(c(1:LOOPCOUNT), d(1:LOOPCOUNT), e(1:LOOPCOUNT)) async(1) wait(2)
          !$acc loop
          DO x = 1, LOOPCOUNT
            e(x) = c(x) + d(x)
          END DO
        !$acc end parallel
        !$acc exit data copyout(e(1:LOOPCOUNT)) async(1)


        DO WHILE (acc_async_test(1) .eqv. .FALSE.)

        END DO

        DO x = 1, LOOPCOUNT
          IF (abs(e(x) - (a(x) + b(x) + d(x))) .GT. PRECISION) THEN
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
