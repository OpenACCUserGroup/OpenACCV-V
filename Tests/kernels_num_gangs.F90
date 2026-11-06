#include "openacc_prof_binding.h"

#ifndef T1
!T1:kernels,reduction,V:2.5-2.7
      LOGICAL FUNCTION test1()
        USE openacc_prof_binding
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x !Iterators
        INTEGER :: results !Data
        INTEGER :: errors = 0

        results = 0
        seen_num_gangs = -1
        CALL register_launch_callback()

        !$acc kernels num_gangs(8)
          !$acc loop gang reduction(+:results)
          DO x = 1, LOOPCOUNT
            results = 1
          END DO
        !$acc end kernels

        IF (seen_num_gangs .NE. 8) THEN
          errors = errors + 1
        END IF

        IF (errors .EQ. 0) THEN
          test1 = .FALSE.
        ELSE
          test1 = .TRUE.
        END IF
      END
#endif

      PROGRAM main
        USE openacc_prof_binding
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
          failed = failed .OR. test1()
        END DO
        IF (failed) THEN
          failcode = failcode + 2 ** 0
          failed = .FALSE.
        END IF
#endif
        CALL EXIT (failcode)
      END PROGRAM
