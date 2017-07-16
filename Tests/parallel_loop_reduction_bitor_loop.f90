      INTEGER FUNCTION test()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.f90"
        INTEGER :: x, y, z, i !Iterators
        INTEGER,DIMENSION(10*LOOPCOUNT):: a, b, b_copy !Data
        REAL(8),DIMENSION(10*LOOPCOUNT):: randoms2
        INTEGER,DIMENSION(10) :: c
        REAL(8),DIMENSION(160*LOOPCOUNT):: randoms
        REAL(8) :: false_margin
        INTEGER :: errors = 0
        INTEGER :: temp

        !Initilization
        CALL RANDOM_SEED()
        CALL RANDOM_NUMBER(randoms)
        CALL RANDOM_NUMBER(randoms2)
        false_margin = exp(log(.5)/n)
        b = FLOOR(6*randoms2)
        b_copy = b
        DO x = 0, 9
          DO y = 1, LOOPCOUNT
            DO z = 1, 16
              IF (randoms(x * 16 * LOOPCOUNT + y * 16 + z - 1) .gt. false_margin) THEN
                temp = 1
                DO i = 1, z
                  temp = temp * 2
                END DO
                a(x * LOOPCOUNT + y) = a(x * LOOPCOUNT + y) + temp
              END IF
            END DO
          END DO
        END DO

        !$acc data copyin(a(1:10*LOOPCOUNT)) copy(b(1:10*LOOPCOUNT), c(1:10))
          !$acc parallel loop gang private(temp)
          DO x = 0, 9
            temp = 0
            !$acc loop worker reduction(ior:temp)
            DO y = 1, LOOPCOUNT
              temp = ior(temp, a(x * LOOPCOUNT + y))
            END DO
            c(x + 1) = temp
            !$acc loop worker
            DO y = 1, LOOPCOUNT
              b(x * LOOPCOUNT + y) = b(x * LOOPCOUNT + y) + c(x + 1)
            END DO
          END DO
        !$acc end data
        
        DO x = 0, 9
          temp = 0
          DO y = 1, LOOPCOUNT
            temp = ior(temp, a(x * LOOPCOUNT + y))
          END DO
          IF (temp .ne. c(x + 1)) THEN
            errors = errors + 1
          END IF
          DO y = 1, LOOPCOUNT
            IF (b(x * LOOPCOUNT + y) .ne. b_copy(x * LOOPCOUNT + y) + c(x + 1)) THEN
              errors = errors + 1
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

      WRITE (*,*) "######## OpenACC Validation Suite V 2.5 ######"
      WRITE (*,*) "## Repetitions:", N
      WRITE (*,*) "## Loop Count :", LOOPCOUNT
      WRITE (*,*) "##############################################"
      WRITE (*,*)

      WRITE (*,*) "--------------------------------------------------"
      WRITE (*,*) "Test of parallel_loop_reduction_bitor_loop"
      WRITE (*,*) "--------------------------------------------------"

      crossfailed=0
      result=1
      WRITE (1,*) "--------------------------------------------------"
      WRITE (1,*) "Test of parallel_loop_reduction_bitor_loop"
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
                                             

