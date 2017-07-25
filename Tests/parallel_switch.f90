      INTEGER FUNCTION test()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.fh"
        INTEGER :: x !Iterators
        INTEGER,DIMENSION(LOOPCOUNT):: a
        REAL(8),DIMENSION(LOOPCOUNT):: b, c, randoms!Data
        INTEGER :: errors = 0

        !Initilization
        CALL RANDOM_SEED()
        CALL RANDOM_NUMBER(b)
        c = 0
        CALL RANDOM_NUMBER(randoms)
        a = floor(randoms * 3)

        !$acc enter data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT), &
            !$acc c(1:LOOPCOUNT))
        !$acc parallel present(a(1:LOOPCOUNT), b(1:LOOPCOUNT), &
            !$acc c(1:LOOPCOUNT))
          !$acc loop
          DO x = 1, LOOPCOUNT
            SELECT CASE (a(x))
              CASE (0)
                c(x) = b(x) * b(x)
              CASE (1)
                c(x) = b(x) / b(x)
              CASE (2)
                c(x) = b(x) / 2
            END SELECT
          END DO
        !$acc end parallel
        !$acc exit data delete(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) &
            !$acc copyout(c(1:LOOPCOUNT))

        DO x = 1, LOOPCOUNT
          SELECT CASE (a(x))
            CASE (0)
              IF (abs(c(x) - (b(x) * b(x))) .gt. PRECISION) THEN
                errors = errors + 1
              END IF
            CASE (1)
              IF (abs(c(x) - (b(x) / b(x))) .gt. PRECISION) THEN
                errors = errors + 1
              END IF
            CASE (2)
              IF (abs(c(x) - (b(x) / 2)) .gt. PRECISION) THEN
                errors = errors + 1
              END IF
          END SELECT
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
      WRITE (*,*) "Test of parallel_switch"
      WRITE (*,*) "--------------------------------------------------"

      crossfailed=0
      result=1
      WRITE (1,*) "--------------------------------------------------"
      WRITE (1,*) "Test of parallel_switch"
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


