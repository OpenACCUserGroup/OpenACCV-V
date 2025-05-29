#ifndef T1
!T1:serial,reduction,combined-constructs,loop,V:2.6-2.7
      LOGICAL FUNCTION test1()
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"
  LOGICAL,DIMENSION(LOOPCOUNT):: a
  LOGICAL:: result, host_result
  REAL(8),DIMENSION(LOOPCOUNT):: randoms
  REAL(8):: false_margin
  INTEGER:: errors, x

  errors = 0
  false_margin = exp(log(.5) / LOOPCOUNT)

  SEEDDIM(1) = 1
# ifdef SEED
  SEEDDIM(1) = SEED
# endif
  CALL RANDOM_SEED(PUT=SEEDDIM)

  CALL RANDOM_NUMBER(randoms)

  DO x = 1, LOOPCOUNT
    IF (randoms(x) .gt. false_margin) THEN
      a(x) = .TRUE.
    ELSE
      a(x) = .FALSE.
    END IF
  END DO

  host_result = .FALSE.
  result = .FALSE.

  DO x = 1, LOOPCOUNT
    host_result = host_result .OR. a(x)
  END DO

  !$acc data copyin(a(1:LOOPCOUNT))
    !$acc serial loop reduction(.OR.: result)
    DO x = 1, LOOPCOUNT
      result = result .OR. a(x)
    END DO
  !$acc end data

  IF (host_result .neqv. result) THEN
    errors = errors + 1
  END IF

        IF (errors .eq. 0) THEN
          test1 = .FALSE.
        ELSE
          test1 = .TRUE.
        END IF
      END
#endif

      PROGRAM main
        IMPLICIT NONE
        INTEGER :: failcode, testrun
        LOGICAL :: failed
        INCLUDE "acc_testsuite.Fh"
#ifndef T1
        LOGICAL :: test1
#endif
        failed = .FALSE.
        failcode = 0
#ifndef T1
        DO testrun = 1, NUM_TEST_CALLS
          failed = failed .or. test1()
        END DO
        IF (failed) THEN
          failcode = failcode + 2 ** 0
          failed = .FALSE.
        END IF
#endif
        CALL EXIT (failcode)
      END PROGRAM
