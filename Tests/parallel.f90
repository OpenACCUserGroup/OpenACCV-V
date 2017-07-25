      INTEGER FUNCTION test()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.fh"
         INTEGER :: x, i_a, i_b, i_c, i_d, i_e, i_f, i_g, i_h, i_i, &
             i_j !Iterators
        INTEGER, PARAMETER :: PRIVATE_LOOPCOUNT = 1024
        REAL(8),DIMENSION(PRIVATE_LOOPCOUNT):: a, b, c !Data
        INTEGER :: errors
        errors = 0

        !Initilization
        CALL RANDOM_SEED()
        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        c = 0

        !$acc data copyin(a(1:PRIVATE_LOOPCOUNT), &
            !$acc b(1:PRIVATE_LOOPCOUNT)) &
            !$acc copy(c(1:PRIVATE_LOOPCOUNT))
          !$acc parallel
            !$acc loop
            DO i_a = 0, 1
              !$acc loop
              DO i_b = 0, 1
                !$acc loop
                DO i_c = 0, 1
                  !$acc loop
                  DO i_d = 0, 1
                    !$acc loop
                    DO i_e = 0, 1
                      !$acc loop
                      DO i_f = 0, 1
                        !$acc loop
                        DO i_g = 0, 1
                          !$acc loop
                          DO i_h = 0, 1
                            !$acc loop
                            DO i_i = 0, 1
                              !$acc loop
                              DO i_j = 0, 1
                                 c(i_a*512 + i_b*256 + i_c*128 + &
                                     i_d*64 + i_e*32 + i_f*16 + i_g*8 &
                                     + i_h*4 + i_i*2 + i_j + 1) = &
                                     a(i_a*512 + i_b*256 + i_c*128 + &
                                     i_d*64 + i_e*32 + i_f*16 + i_g*8 &
                                     + i_h*4 + i_i*2 + i_j + 1) + &
                                     b(i_a*512 + i_b*256 + i_c*128 + &
                                     i_d*64 + i_e*32 + i_f*16 + i_g*8 &
                                     + i_h*4 + i_i*2 + i_j + 1)
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
          !$acc end parallel
        !$acc end data
        DO x = 1, PRIVATE_LOOPCOUNT
          IF (abs(c(x) - (a(x) + b(x))) .gt. PRECISION) THEN
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
      WRITE (*,*) "Test of parallel"
      WRITE (*,*) "--------------------------------------------------"

      crossfailed=0
      result=1
      WRITE (1,*) "--------------------------------------------------"
      WRITE (1,*) "Test of parallel"
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


