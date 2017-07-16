      INTEGER FUNCTION test()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.f90"
        INTEGER :: x, y !Iterators
        LOGICAL,DIMENSION(10*LOOPCOUNT):: a !Data
        LOGICAL,DIMENSION(10):: b
        LOGICAL,DIMENSION(10):: has_false
        LOGICAL :: temp
        REAL(8) :: false_margin
        REAL(8),DIMENSION(10*LOOPCOUNT) :: randoms
        INTEGER :: errors = 0

        !Initilization
        CALL RANDOM_SEED()
        CALL RANDOM_NUMBER(randoms)
        false_margin = exp(log(.5) / LOOPCOUNT)
        DO x = 1, 10
          has_false(x) = .FALSE.
          b(x) = .TRUE.
        END DO

        DO x = 0, 9
          DO y = 1, LOOPCOUNT
            IF (randoms(x * LOOPCOUNT + y) .lt. false_margin) THEN
              a(x * LOOPCOUNT + y) = .TRUE.
            ELSE
              a(x * LOOPCOUNT + y) = .FALSE.
              has_false(x + 1) = .TRUE.
            END IF
          END DO
        END DO

        !$acc data copyin(a(1:10*LOOPCOUNT)) copy(b(1:10))
          !$acc parallel loop private(temp)
          DO x = 0, 9
            temp = .TRUE.
            !$acc loop vector reduction(.AND.:temp)
            DO y = 1, LOOPCOUNT
              temp = temp .AND. a(x * LOOPCOUNT + y)
            END DO
            b(x + 1) = temp
          END DO
        !$acc end data

        DO x = 0, 9
          temp = .FALSE.
          DO y = 1, LOOPCOUNT
            IF (a(x * LOOPCOUNT + y) .eqv. .FALSE.) THEN
              temp = .TRUE.
            END IF
          END DO
          IF (temp .neqv. has_false(x + 1)) THEN
            errors = 1
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
      WRITE (*,*) "Test of parallel_loop_reduction_and_vector_loop"
      WRITE (*,*) "--------------------------------------------------"

      crossfailed=0
      result=1
      WRITE (1,*) "--------------------------------------------------"
      WRITE (1,*) "Test of parallel_loop_reduction_and_vector_loop"
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
                                             

