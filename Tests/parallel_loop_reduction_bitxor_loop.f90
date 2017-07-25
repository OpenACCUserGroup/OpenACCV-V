      INTEGER FUNCTION test()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.fh"
        INTEGER :: x, y !Iterators
        INTEGER,DIMENSION(10*LOOPCOUNT):: a, b, b_copy !Data
        REAL(8),DIMENSION(10*LOOPCOUNT):: randoms, randoms2
        INTEGER,DIMENSION(10):: c
        REAL*8 :: RAND
        INTEGER :: errors = 0
        INTEGER :: temp

        !Initilization
        CALL RANDOM_SEED()
        CALL RANDOM_NUMBER(randoms)
        a = FLOOR(randoms*1000000)
        CALL RANDOM_NUMBER(randoms2)
        b = FLOOR(randoms2*1000000)
        b_copy = b
        c = 0

        !$acc data copyin(a(1:10*LOOPCOUNT)) copy(b(1:10*LOOPCOUNT), &
            !$acc c(1:10))
          !$acc parallel loop gang private(temp)
          DO x = 0, 9
            temp = 0
            !$acc loop worker reduction(ieor:temp)
            DO y = 1, LOOPCOUNT
              temp = ieor(temp, a(x * LOOPCOUNT + y))
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
            temp = ieor(temp, a(x * LOOPCOUNT + y))
          END DO
          IF (temp .ne. c(x + 1)) THEN
            errors = errors + 1
          END IF
          DO y = 1, LOOPCOUNT
              IF (b(x * LOOPCOUNT + y) .ne. b_copy(x * LOOPCOUNT + y) &
                  + c(x + 1)) THEN
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
      WRITE (*,*) "Test of parallel_loop_reduction_bitxor_loop"
      WRITE (*,*) "--------------------------------------------------"

      crossfailed=0
      result=1
      WRITE (1,*) "--------------------------------------------------"
      WRITE (1,*) "Test of parallel_loop_reduction_bitxor_loop"
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


