      INTEGER FUNCTION test()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.fh"
        INTEGER :: x !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, a_copy, b, b_copy, c !Data
        INTEGER :: errors = 0
        INTEGER,DIMENSION(1):: devtest
        LOGICAL :: dev = .TRUE.
        LOGICAL :: cpu = .FALSE.
        devtest(1) = 1

        !$acc enter data copyin(devtest(1:1))
        !$acc parallel
          devtest(1) = 0
        !$acc end parallel

        !Initilization
        CALL RANDOM_SEED()
        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        c = 0

        !$acc enter data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) if(dev)
        !$acc data create(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) &
            !$acc copyout(c(1:LOOPCOUNT))
          !$acc parallel
            !$acc loop
            DO x = 1, LOOPCOUNT
              c(x) = a(x) + b(x)
            END DO
          !$acc end parallel
        !$acc end data
        !$acc exit data delete(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) if(dev)
        DO x = 1, LOOPCOUNT
          IF (abs(c(x) - (a(x) + b(x))) .gt. PRECISION) THEN
            errors = errors + 1
          END IF
        END DO

        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        c = 0

        !$acc enter data create(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) if(cpu)
        !$acc data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) &
            !$acc copyout(c(1:LOOPCOUNT))
          !$acc parallel
            !$acc loop
            DO x = 1, LOOPCOUNT
              c(x) = a(x) + b(x)
            END DO
          !$acc end parallel
        !$acc end data
        !$acc exit data copyout(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) if(cpu)

        DO x = 1, LOOPCOUNT
          IF (abs(c(x) - (a(x) + b(x))) .gt. PRECISION) THEN
            errors = errors + 1
          END IF
        END DO

        IF (devtest(1) .eq. 1) THEN
          CALL RANDOM_NUMBER(a)
          a_copy = a
          CALL RANDOM_NUMBER(b)
          b_copy = b
          c = 0

          !$acc enter data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) &
              !$acc if(dev)

          a = 0
          b = 0

          !$acc data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) &
              !$acc copyout(c(1:LOOPCOUNT))
            !$acc parallel
              !$acc loop
              DO x = 1, LOOPCOUNT
                c(x) = a(x) + b(x)
              END DO
            !$acc end parallel
          !$acc end data
          !$acc exit data delete(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) &
              !$acc if(dev)

          DO x = 1, LOOPCOUNT
             IF (abs(c(x) - (a_copy(x) + b_copy(x))) .gt. PRECISION) &
                 THEN
              errors = errors + 1
            END IF
          END DO

          CALL RANDOM_NUMBER(a)
          CALL RANDOM_NUMBER(b)
          c = 0

          !$acc enter data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) &
              !$acc if(cpu)
          CALL RANDOM_NUMBER(a)
          a_copy = a
          CALL RANDOM_NUMBER(b)
          b_copy = b

          !$acc data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) &
              !$acc copyout(c(1:LOOPCOUNT))
            !$acc parallel
              !$acc loop
              DO x = 1, LOOPCOUNT
                c(x) = a(x) + b(x)
              END DO
            !$acc end parallel
          !$acc end data
          !$acc exit data copyout(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) &
              !$acc if(cpu)

          DO x = 1, LOOPCOUNT
             IF (abs(c(x) - (a_copy(x) + b_copy(x))) .gt. PRECISION) &
                 THEN
              errors = errors + 1
            END IF
          END DO

          CALL RANDOM_NUMBER(a)
          CALL RANDOM_NUMBER(b)
          c = 0

          !$acc enter data create(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) &
              !$acc if(cpu)
          !$acc data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) &
              !$acc copyout(c(1:LOOPCOUNT))
            !$acc parallel
              !$acc loop
              DO x = 1, LOOPCOUNT
                c(x) = a(x) + b(x)
              END DO
            !$acc end parallel
          !$acc end data

          DO x = 1, LOOPCOUNT
            IF (abs(c(x) - (a(x) + b(x))) .gt. PRECISION) THEN
              errors = errors + 1
            END IF
          END DO
        END IF

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
      WRITE (*,*) "Test of enter_data_if"
      WRITE (*,*) "--------------------------------------------------"

      crossfailed=0
      result=1
      WRITE (1,*) "--------------------------------------------------"
      WRITE (1,*) "Test of enter_data_if"
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


