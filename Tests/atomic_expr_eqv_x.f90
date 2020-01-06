      INTEGER FUNCTION test()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.fh"
        INTEGER :: x, y !Iterators
        REAL(8),DIMENSION(LOOPCOUNT, 10):: randoms
        LOGICAL,DIMENSION(LOOPCOUNT, 10):: a !Data
        LOGICAL,DIMENSION(LOOPCOUNT):: totals, totals_comparison
        INTEGER :: errors = 0

        !Initilization
        CALL RANDOM_SEED()
        CALL RANDOM_NUMBER(randoms)
        DO x = 1, LOOPCOUNT
          DO y = 1, 10
            IF (randoms(x, y) > .5) THEN
              a(x, y) = .TRUE.
            ELSE
              a(x, y) = .FALSE.
            END IF
          END DO
        END DO

        totals = .FALSE.
        totals_comparison = .FALSE.

        !$acc data copyin(a(1:LOOPCOUNT,1:10)) copy(totals(1:LOOPCOUNT))
          !$acc parallel
            !$acc loop
            DO x = 1, LOOPCOUNT
              DO y = 1, 10
                !$acc atomic
                  totals(x) = a(x, y) .EQV. totals(x)
              END DO
            END DO
          !$acc end parallel
        !$acc end data

        DO x = 1, LOOPCOUNT
          DO y = 1, 10
            totals_comparison(x) = totals_comparison(x) .EQV. a(x, y)
          END DO
        END DO

        DO x = 1, LOOPCOUNT
          IF (totals_comparison(x) .NEQV. totals(x)) THEN
            errors = errors + 1
            WRITE(*, *) totals_comparison(x)
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
