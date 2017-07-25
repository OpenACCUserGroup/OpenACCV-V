      INTEGER FUNCTION test()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.fh"
        INTEGER :: x !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, b, c !Data
        INTEGER :: errors = 0
        INTEGER,DIMENSION(1):: devtest

        devtest(1) = 1
        !$acc enter data copyin(devtest(1:1))
        !$acc parallel present(devtest(1:1))
          devtest(1) = 0
        !$acc end parallel

        !Initilization
        CALL RANDOM_SEED()
        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        c = 0

        IF (devtest(1) .eq. 1) THEN
          !$acc enter data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT), &
             !$acc c(1:LOOPCOUNT))
          !$acc data copyin(c(1:LOOPCOUNT))
            !$acc parallel
              !$acc loop
              DO x = 1, LOOPCOUNT
                c(x) = c(x) + a(x) + b(x)
              END DO
            !$acc end parallel
            !$acc exit data delete(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) &
               !$acc copyout(c(1:LOOPCOUNT))
          !$acc end data

          DO x = 1, LOOPCOUNT
            IF (abs(c(x)) .gt. PRECISION) THEN
              errors = errors + 1
              EXIT
            END IF
          END DO

          CALL RANDOM_NUMBER(a)
          CALL RANDOM_NUMBER(b)
          c = 0
        END IF

        !$acc enter data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT), &
           !$acc c(1:LOOPCOUNT))
        !$acc data copyin(c(1:LOOPCOUNT))
          !$acc parallel
            !$acc loop
            DO x = 1, LOOPCOUNT
              c(x) = c(x) + a(x) + b(x)
            END DO
          !$acc end parallel
        !$acc end data
        !$acc exit data copyout(c(1:LOOPCOUNT)) &
           !$acc delete(a(1:LOOPCOUNT), b(1:LOOPCOUNT))

        DO x = 1, LOOPCOUNT
          IF (abs(c(x) - (a(x) + b(x))) .gt. PRECISION) THEN
            errors = errors + 2
            EXIT
          END IF
        END DO

        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        c = 0

        !$acc enter data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT), &
           !$acc c(1:LOOPCOUNT))
        !$acc enter data copyin(c(1:LOOPCOUNT))
        !$acc parallel
          !$acc loop
          DO x = 1, LOOPCOUNT
            c(x) = c(x) + a(x) + b(x)
          END DO
        !$acc end parallel
        !$acc exit data delete(c(1:LOOPCOUNT))
        !$acc exit data delete(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) &
           !$acc copyout(c(1:LOOPCOUNT))

        DO x = 1, LOOPCOUNT
          IF (abs(c(x) - (a(x) + b(x))) .gt. PRECISION) THEN
            errors = errors + 4
            EXIT
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
      WRITE (*,*) "Test of exit_data_copyout_reference_counts"
      WRITE (*,*) "--------------------------------------------------"

      crossfailed=0
      result=1
      WRITE (1,*) "--------------------------------------------------"
      WRITE (1,*) "Test of exit_data_copyout_reference_counts"
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


