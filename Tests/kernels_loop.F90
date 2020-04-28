#ifndef T1
!T1:kernels,loop,V:1.0-2.7
      LOGICAL FUNCTION test1()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9 !Iterators
        REAL(8),DIMENSION(1024):: a, b !Data
        INTEGER :: errors = 0

        !Initilization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        CALL RANDOM_NUMBER(a)
        b = 0

        !$acc data copyin(a(1:1024)) copyout(b(1:1024))
          !$acc kernels
            !$acc loop
            DO _0 = 0, 1
              !$acc loop
              DO _1 = 0, 1
                !$acc loop
                DO _2 = 0, 1
                  !$acc loop
                  DO _3 = 0, 1
                    !$acc loop
                    DO _4 = 0, 1
                      !$acc loop
                      DO _5 = 0, 1
                        !$acc loop
                        DO _6 = 0, 1
                          !$acc loop
                          DO _7 = 0, 1
                            !$acc loop
                            DO _8 = 0, 1
                              !$acc loop
                              DO _9 = 1, 2
                                b(_0*512+_1*256+_2*128+_3*64+_4*32+_5*16+_6*8+_7*4+_8*2+_9)=a(_0*512+_1*256+_2*128+_3*64+_4*32+_5*16+_6*8+_7*4+_8*2+_9)
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
