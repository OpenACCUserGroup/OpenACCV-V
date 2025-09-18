#ifndef T1
!T1:kernels,loop,V:1.0-2.7
      LOGICAL FUNCTION test1()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x, x_0, x_1, x_2, x_3, x_4, x_5, x_6, x_7, x_8, x_9 !Iterators
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
            DO x_0 = 0, 1
              !$acc loop
              DO x_1 = 0, 1
                !$acc loop
                DO x_2 = 0, 1
                  !$acc loop
                  DO x_3 = 0, 1
                    !$acc loop
                    DO x_4 = 0, 1
                      !$acc loop
                      DO x_5 = 0, 1
                        !$acc loop
                        DO x_6 = 0, 1
                          !$acc loop
                          DO x_7 = 0, 1
                            !$acc loop
                            DO x_8 = 0, 1
                              !$acc loop
                              DO x_9 = 1, 2
                                b(x_0*512+x_1*256+x_2*128+x_3*64+x_4*32+x_5*16+x_6*8+x_7*4+x_8*2+x_9) &
                                  = a(x_0*512+x_1*256+x_2*128+x_3*64+x_4*32+x_5*16+x_6*8+x_7*4+x_8*2+x_9)
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
