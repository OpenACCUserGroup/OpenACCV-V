      INTEGER FUNCTION test()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.fh"
         INTEGER :: x, v_0, v_1, v_2, v_3, v_4, v_5, v_6, v_7, v_8, &
            v_9 !Iterators
        REAL(8),DIMENSION(1024):: a, b !Data
        INTEGER :: errors = 0

        !Initilization
        CALL RANDOM_SEED()
        CALL RANDOM_NUMBER(a)
        b = 0

        !$acc data copyin(a(1:1024)) copyout(b(1:1024))
          !$acc kernels
            !$acc loop
            DO v_0 = 0, 1
              !$acc loop
              DO v_1 = 0, 1
                !$acc loop
                DO v_2 = 0, 1
                  !$acc loop
                  DO v_3 = 0, 1
                    !$acc loop
                    DO v_4 = 0, 1
                      !$acc loop
                      DO v_5 = 0, 1
                        !$acc loop
                        DO v_6 = 0, 1
                          !$acc loop
                          DO v_7 = 0, 1
                            !$acc loop
                            DO v_8 = 0, 1
                              !$acc loop
                              DO v_9 = 1, 2
                                 b(v_0 * 512 + v_1 * 256 + v_2 * 128 &
                                    + v_3 * 64 + v_4 * 32 + v_5 * 16 &
                                    + v_6 * 8 + v_7 * 4 + v_8 * 2 + &
                                    v_9) = a(v_0 * 512 + v_1 * 256 + &
                                    v_2 * 128 + v_3 * 64 + v_4 * 32 + &
                                    v_5 * 16 + v_6 * 8 + v_7 * 4 + &
                                    v_8 * 2 + v_9)
                              END DO
                            END DO
                          END DO
                        END DO
                      END DO
                    END DO
                  END DO
                END DO
              END DO
            END DO
          !$acc end kernels
        !$acc end data

        DO x = 1, 1024
          IF (abs(a(x) - b(x)) .gt. PRECISION) THEN
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
      WRITE (*,*) "Test of kernels_loop"
      WRITE (*,*) "--------------------------------------------------"

      crossfailed=0
      result=1
      WRITE (1,*) "--------------------------------------------------"
      WRITE (1,*) "Test of kernels_loop"
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


