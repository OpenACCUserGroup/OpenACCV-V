! Set up Fortran bindings for the OpenACC profiling interface because the
! OpenACC specification only defines C bindings and thus at this time
! there is no portable way to rely on them being supplied by the openacc
! module. This allows observing num_gangs on launch in a portable way.
MODULE openacc_prof_binding
  USE ISO_C_BINDING
  IMPLICIT NONE

  INTEGER, VOLATILE :: launch_count = 0
  INTEGER(C_SIZE_T), VOLATILE :: seen_num_gangs = -1
  INTEGER(C_SIZE_T), VOLATILE :: seen_num_workers = -1
  INTEGER(C_SIZE_T), VOLATILE :: seen_vector_length = -1
  LOGICAL, VOLATILE :: launch_cb_registered = .FALSE.

  ! acc_event_t (OpenACC 3.4 Section 5.1)
  INTEGER(C_INT), PARAMETER :: acc_ev_enqueue_launch_start = 18

  ! acc_register_t (OpenACC 3.4 Section 5.4.1)
  INTEGER(C_INT), PARAMETER :: acc_reg = 0

  ! acc_launch_event_info (OpenACC 3.4 Section 5.2.2)
  TYPE, BIND(C) :: acc_launch_event_info
    INTEGER(C_INT) :: event_type
    INTEGER(C_INT) :: valid_bytes
    INTEGER(C_INT) :: parent_construct
    INTEGER(C_INT) :: implicit
    TYPE(C_PTR) :: tool_info
    TYPE(C_PTR) :: kernel_name
    INTEGER(C_SIZE_T) :: num_gangs
    INTEGER(C_SIZE_T) :: num_workers
    INTEGER(C_SIZE_T) :: vector_length
    TYPE(C_PTR) :: num_gangs_per_dim
  END TYPE acc_launch_event_info

  ! acc_prof_register (OpenACC 3.4 Section 5.3.5). This is the
  ! backward-compatible name of acc_callback_register.
  INTERFACE
    SUBROUTINE acc_prof_register(eventtype, cb, reg_mode) BIND(C)
      IMPORT
      INTEGER(C_INT), VALUE :: eventtype
      TYPE(C_FUNPTR), VALUE :: cb
      INTEGER(C_INT), VALUE :: reg_mode
    END SUBROUTINE acc_prof_register
  END INTERFACE

  ABSTRACT INTERFACE
    ! acc_prof_reg (OpenACC 3.4 Section 5.4.1)
    SUBROUTINE acc_prof_reg(eventtype, cb, reg_mode) BIND(C)
      IMPORT
      INTEGER(C_INT), VALUE :: eventtype
      TYPE(C_FUNPTR), VALUE :: cb
      INTEGER(C_INT), VALUE :: reg_mode
    END SUBROUTINE acc_prof_reg
  END INTERFACE

CONTAINS

  ! acc_prof_callback (OpenACC 3.4 Section 5.2)
  SUBROUTINE acc_prof_callback(prof_info, event_info, api_info) BIND(C)
    TYPE(C_PTR), VALUE :: prof_info, event_info, api_info
    TYPE(acc_launch_event_info), POINTER :: launch_event
    CALL C_F_POINTER(event_info, launch_event)
    launch_count = launch_count + 1
    ! One region can enqueue more than one kernel. Keep the launch
    ! with the largest gang count.
    IF (launch_event%num_gangs .GT. seen_num_gangs) THEN
      seen_num_gangs = launch_event%num_gangs
      seen_num_workers = launch_event%num_workers
      seen_vector_length = launch_event%vector_length
    END IF
  END SUBROUTINE acc_prof_callback

  ! Application-controlled registration (OpenACC 3.4 Section 5.3.5).
  ! Some runtimes define acc_register_library themselves and never call
  ! the copy in the executable, so the test calls this before launch.
  SUBROUTINE register_launch_callback()
    IF (launch_cb_registered) RETURN
    launch_cb_registered = .TRUE.
    CALL acc_prof_register(acc_ev_enqueue_launch_start, &
                           C_FUNLOC(acc_prof_callback), acc_reg)
  END SUBROUTINE register_launch_callback

  ! acc_register_library (OpenACC 3.4 Section 5.4.1)
  SUBROUTINE acc_register_library(reg, unreg, lookup) &
      BIND(C, NAME="acc_register_library")
    TYPE(C_FUNPTR), VALUE :: reg, unreg, lookup
    PROCEDURE(acc_prof_reg), POINTER :: register_cb
    IF (launch_cb_registered) RETURN
    launch_cb_registered = .TRUE.
    CALL C_F_PROCPOINTER(reg, register_cb)
    CALL register_cb(acc_ev_enqueue_launch_start, &
                     C_FUNLOC(acc_prof_callback), acc_reg)
  END SUBROUTINE acc_register_library

END MODULE openacc_prof_binding
