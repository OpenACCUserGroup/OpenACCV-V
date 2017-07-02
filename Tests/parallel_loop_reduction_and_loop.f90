      INTEGER FUNCTION test()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.f90"
        INTEGER :: x, y !Iterators
        LOGICAL,DIMENSION(10*LOOPCOUNT):: a, a_copy !Data
        LOGICAL,DIMENSION(10) :: results, has_false
        LOGICAL :: host_results = .TRUE.
        LOGICAL :: temp = .TRUE.
        REAL(8),DIMENSION(10*LOOPCOUNT):: randoms
        REAL(8) :: false_margin = exp(log(.5) / LOOPCOUNT)
        INTEGER :: errors = 0
        !Initilization
        CALL RANDOM_SEED()
        CALL RANDOM_NUMBER(randoms)
        DO x = 1, 10
          results(x) = .TRUE.
          has_false(x) = .FALSE.
        END DO
        DO x = 1, 10*LOOPCOUNT
          IF (randoms(x) .lt. false_margin) THEN
            a(x) = .TRUE.
            a_copy(x) = .TRUE.
          ELSE
            a(x) = .FALSE.
            a_copy(x) = .FALSE.
            has_false(x / LOOPCOUNT) = .TRUE.
          END IF
        END DO

        !$acc data copy(a(1:10*LOOPCOUNT), results(1:10))
          !$acc parallel loop gang
          DO x = 0, 9
            temp = .TRUE.
            !$acc loop worker reduction(.AND.:temp)
            DO y = 1, LOOPCOUNT
              temp = temp .AND. a(x * LOOPCOUNT + y)
            END DO
            results(x + 1) = temp
            !$acc loop worker
            DO y = 1, LOOPCOUNT
              IF (temp .eqv. .TRUE.) THEN
                IF (a(x * LOOPCOUNT + y) .eqv. .TRUE.) THEN
                  a(x * LOOPCOUNT + y) = .FALSE.
                ELSE
                  a(x * LOOPCOUNT + y) = .TRUE.
                END IF
              END IF
            END DO
          END DO
        !$acc end data
        DO x = 0, 9
          temp = .TRUE.
          DO y = 1, LOOPCOUNT
            temp = temp .AND. a_copy(x * LOOPCOUNT + y)
          END DO
          IF (temp .neqv. results(x + 1)) THEN
            errors = errors + 1
          END IF
          DO y = 1, LOOPCOUNT
            IF (temp .eqv. .TRUE.) THEN
              IF (a(x * LOOPCOUNT + y) .eqv. a_copy(x * LOOPCOUNT + y)) THEN
                errors = errors + 1
              END IF
            ELSE
              IF (a(x * LOOPCOUNT + y) .neqv. a_copy(x * LOOPCOUNT + y)) THEN
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
