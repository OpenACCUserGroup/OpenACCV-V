#ifndef T1
!T1:serial,private,reduction,combined-constructs,loop,V:2.6-2.7
      LOGICAL FUNCTION test1()
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"
  REAL(8),DIMENSION(10, LOOPCOUNT):: a, b
  REAL(8),DIMENSION(LOOPCOUNT):: totals, host_totals
  REAL(8):: temp
  INTEGER:: errors, x, y

  errors = 0

  SEEDDIM(1) = 1
  #ifdef SEED
  SEEDDIM(1) = SEED
  #endif
  CALL RANDOM_SEED(PUT=SEEDDIM)

  CALL RANDOM_NUMBER(a)
  CALL RANDOM_NUMBER(b)

  host_totals = 1
  DO y = 1, LOOPCOUNT
    DO x = 1, 10
      host_totals(y) = host_totals(y) * (a(x, y) + b(x, y))
    END DO
  END DO

  !$acc data copyin(a(1:10, 1:LOOPCOUNT), b(1:10, 1:LOOPCOUNT)) copyout(totals(1:LOOPCOUNT))
    !$acc serial loop private(temp)
    DO y = 1, LOOPCOUNT
      temp = 1
      !$acc loop vector reduction(*:temp)
      DO x = 1, 10
        temp = temp * (a(x, y) + b(x, y))
      END DO
      totals(y) = temp
    END DO
  !$acc end data

  DO y = 1, LOOPCOUNT
    IF (abs(host_totals(y) - totals(y)) .gt. PRECISION) THEN
      errors = errors + 1
    END IF
  END DO

        IF (errors .eq. 0) THEN
          test1 = .FALSE.
        ELSE
          test1 = .TRUE.
        END IF
      END
#endif

      PROGRAM serial_loop_reduction_multiply_vector_loop
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
