      INTEGER FUNCTION test()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.fh"
        INTEGER :: x !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, b, c, d, e, f, g !Data
        INTEGER :: errors = 0

        !Initilization
        CALL RANDOM_SEED()
        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        c = 0
        CALL RANDOM_NUMBER(d)
        CALL RANDOM_NUMBER(e)
        f = 0
        g = 0
        !$acc enter data create(g(1:LOOPCOUNT))
        !$acc data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT), &
            !$acc d(1:LOOPCOUNT), e(1:LOOPCOUNT)) &
            !$acc copy(c(1:LOOPCOUNT), f(1:LOOPCOUNT))
          !$acc kernels async(1)
            !$acc loop
            DO x = 1, LOOPCOUNT
              c(x) = a(x) + b(x)
            END DO
          !$acc end kernels
          !$acc kernels async(2)
            !$acc loop
            DO x = 1, LOOPCOUNT
              f(x) = d(x) + e(x)
            END DO
          !$acc end kernels
          !$acc kernels wait(1, 2) async(3)
            !$acc loop
            DO x = 1, LOOPCOUNT
              g(x) = c(x) + f(x)
            END DO
          !$acc end kernels
          !$acc wait(1, 2)
        !$acc end data
        !$acc update host(g(1:LOOPCOUNT)) async(3)
        DO x = 1, LOOPCOUNT
          IF (abs(c(x) - (a(x) + b(x))) .gt. PRECISION) THEN
            errors = errors + 1
            WRITE(*, *) x, " a: ", c(x), " = ", a(x), " + ", b(x)
          END IF
          IF (abs(f(x) - (d(x) + e(x))) .gt. PRECISION) THEN
            errors = errors + 1
            WRITE(*, *) x, " b: ", f(x), " = ", d(x), " + ", e(x)
          END IF
        END DO
        !$acc wait(3)
        DO x = 1, LOOPCOUNT
          IF (abs(g(x) - (c(x) + f(x))) .gt. PRECISION) THEN
            errors = errors + 1
            WRITE(*, *) x, " c: ", g(x), " = ", c(x), " + ", f(x)
          END IF
        END DO
        !$acc exit data delete(g(1:LOOPCOUNT))
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
      WRITE (*,*) "Test of kernels_async"
      WRITE (*,*) "--------------------------------------------------"

      crossfailed=0
      result=1
      WRITE (1,*) "--------------------------------------------------"
      WRITE (1,*) "Test of kernels_async"
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


