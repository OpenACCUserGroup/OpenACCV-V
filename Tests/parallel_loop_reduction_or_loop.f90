      INTEGER FUNCTION test()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.fh"
        INTEGER :: x, y !Iterators
        LOGICAL,DIMENSION(10*LOOPCOUNT):: a, a_copy !Data
        LOGICAL,DIMENSION(10) :: results
        LOGICAL :: temp = .FALSE.
        REAL(8),DIMENSION(10 * LOOPCOUNT) :: random
        REAL(8) :: false_margin = exp(log(.5) / n)
        INTEGER :: errors = 0

        !Initilization
        DO x = 1, 10 * LOOPCOUNT
          IF (random(x) .gt. false_margin) THEN
            a(x) = .TRUE.
          ELSE
            a(x) = .FALSE.
          END IF
          a_copy(x) = a(x)
        END DO

        DO x = 1, 10
          results(x) = .FALSE.
        END DO

        !$acc data copy(a(1:10*LOOPCOUNT), results(1:10))
          !$acc parallel loop gang private(temp)
          DO x = 0, 9
            temp = .FALSE.
            !$acc loop worker reduction(.OR.:temp)
            DO y = 1, LOOPCOUNT
              temp = temp .OR. a(x * LOOPCOUNT + y)
            END DO
            results(x + 1) = temp
            !$acc loop worker
            DO y = 1, LOOPCOUNT
              IF (results(x + 1) .eqv. .TRUE.) THEN
                IF(a(x * LOOPCOUNT + y) .eqv. .TRUE.) THEN
                  a(x * LOOPCOUNT + y) = .FALSE.
                ELSE
                  a(x * LOOPCOUNT + y) = .TRUE.
                END IF
              END IF
            END DO
          END DO
        !$acc end data

        DO x = 0, 9
          temp = .FALSE.
          DO y = 1, LOOPCOUNT
            temp = temp .OR. a_copy(x * LOOPCOUNT + y)
          END DO
          IF (temp .neqv. results(x + 1)) THEN
            errors = errors + 1
          END IF
          DO y = 1, LOOPCOUNT
            IF (temp .eqv. .FALSE.) THEN
                IF (a(x * LOOPCOUNT + y) .neqv. a_copy(x * LOOPCOUNT &
                    + y)) THEN
                errors = errors + 1
              END IF
            ELSE
                IF (a(x * LOOPCOUNT + y) .eqv. a_copy(x * LOOPCOUNT + &
                    y)) THEN
                errors = errors + 1
              END IF
            END IF
          END DO
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
      WRITE (*,*) "Test of parallel_loop_reduction_or_loop"
      WRITE (*,*) "--------------------------------------------------"

      crossfailed=0
      result=1
      WRITE (1,*) "--------------------------------------------------"
      WRITE (1,*) "Test of parallel_loop_reduction_or_loop"
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


