RECURSIVE FUNCTION IS_POSSIBLE(a, b, length, init) RESULT(POSSIBLE)
  INTEGER, INTENT(IN) :: length
  INTEGER, INTENT(IN) :: init
  INTEGER,DIMENSION(length), INTENT(IN) :: a
  INTEGER,DIMENSION(length), INTENT(IN) :: b
  INTEGER,DIMENSION(length - 1) :: passed_a
  INTEGER,DIMENSION(length - 1) :: passed_b
  INTEGER :: holder
  LOGICAL :: POSSIBLE
  INTEGER :: x, y

  IF (length .eq. 0) THEN
    POSSIBLE = .TRUE.
    RETURN
  END IF
  POSSIBLE = .FALSE.

  DO x = 1, length
    IF (b(x) .eq. ieor(init, a(x))) THEN
      DO y = 1, x - 1
        passed_a(y) = a(y)
        passed_b(y) = b(y)
      END DO
      DO y = x + 1, length
        passed_a(y - 1) = a(y)
        passed_b(y - 1) = b(y)
      END DO
      holder = b(x)
      IF (IS_POSSIBLE(passed_a, passed_b, length - 1, holder)) THEN
        POSSIBLE = .TRUE.
        RETURN
      END IF
    END IF
  END DO
END FUNCTION IS_POSSIBLE

INTEGER FUNCTION test()
  IMPLICIT NONE
  INCLUDE "acc_testsuite.fh"
  INTEGER :: x, y, z !Iterators
  INTEGER,DIMENSION(LOOPCOUNT, 10):: a, b !Data
  REAL(8),DIMENSION(LOOPCOUNT, 10, 8):: randoms
  INTEGER,DIMENSION(LOOPCOUNT):: totals, totals_comparison
  INTEGER,DIMENSION(10):: passed_a, passed_b
  INTEGER:: init
  LOGICAL IS_POSSIBLE
  INTEGER :: errors = 0

  !Initilization
  CALL RANDOM_SEED()
  CALL RANDOM_NUMBER(randoms)
  a = 0
  DO x = 1, LOOPCOUNT
    DO y = 1, 10
      DO z = 1, 8
        IF (randoms(x, y, z) .gt. .5) THEN
          a(x, y) = a(x, y) + ISHFT(1, z - 1)
        END IF
      END DO
    END DO
  END DO
  totals = 0
  totals_comparison = 0

  !$acc data copyin(a(1:LOOPCOUNT, 1:10)) copy(totals(1:LOOPCOUNT)) copyout(b(1:LOOPCOUNT, 1:10))
    !$acc parallel
      !$acc loop
      DO x = 1, LOOPCOUNT
        DO y = 1, 10
          !$acc atomic capture
            totals(x) = ieor(a(x, y), totals(x))
            b(x, y) = totals(x)
          !$acc end atomic
        END DO
      END DO
    !$acc end parallel
  !$acc end data
  DO x = 1, LOOPCOUNT
    DO y = 1, 10
      totals_comparison(x) = ieor(totals_comparison(x), a(x, y))
    END DO
  END DO
  DO x = 1, LOOPCOUNT
    IF (totals_comparison(x) .NE. totals(x)) THEN
      errors = errors + 1
      WRITE(*, *) totals_comparison(x)
    END IF
  END DO

  DO x = 1, LOOPCOUNT
    DO y = 1, 10
      passed_a(y) = a(x, y)
      passed_b(y) = b(x, y)
    END DO
    init = 0
    IF (IS_POSSIBLE(passed_a, passed_b, 10, init) .eqv. .FALSE.) THEN
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
