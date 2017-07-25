      INTEGER FUNCTION test()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.fh"
        INTEGER :: x, y, z !Iterators
        INTEGER,DIMENSION(10 * LOOPCOUNT):: a !Data
        INTEGER,DIMENSION(10) :: b
        REAL(8) :: false_margin
        REAL(8),DIMENSION(160 * LOOPCOUNT) :: randoms
        INTEGER :: errors = 0
        INTEGER :: temp

        !Initilization
        CALL RANDOM_SEED()
        CALL RANDOM_NUMBER(randoms)
        false_margin = exp(log(.5) / n)
        DO x = 1, 10 * LOOPCOUNT
          DO y = 1, 16
             IF (randoms(y * 10 * LOOPCOUNT + y - 1) .gt. &
                false_margin) THEN
              temp = 1
              DO z = 1, y
                temp = temp * 2
              END DO
              a(x) = a(x) + temp
            END IF
          END DO
        END DO

        !$acc data copyin(a(1:10*LOOPCOUNT)) copy(b(1:10))
          !$acc parallel loop private(temp)
          DO x = 0, 9
            temp = 0
            !$acc loop vector reduction(ior:temp)
            DO y = 1, LOOPCOUNT
              temp = ior(temp, a(x * LOOPCOUNT + y))
            END DO
            b(x + 1) = temp
          END DO
        !$acc end data

        DO x = 0, 9
          temp = 0
          DO y = 1, LOOPCOUNT
            temp = ior(temp, a(x * LOOPCOUNT + y))
          END DO
          IF (temp .ne. b(x + 1)) THEN
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
      WRITE (*,*) "Test of parallel_loop_reduction_bitor_vector_loop"
      WRITE (*,*) "--------------------------------------------------"

      crossfailed=0
      result=1
      WRITE (1,*) "--------------------------------------------------"
      WRITE (1,*) "Test of parallel_loop_reduction_bitor_vector_loop"
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


