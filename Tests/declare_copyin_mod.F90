MODULE DECLARE_COPYIN_MOD
  INTEGER, DIMENSION(0:9) :: fixed_size_array = 5
  INTEGER :: scalar = 10
  !$acc declare copyin(fixed_size_array, scalar)
END MODULE DECLARE_COPYIN_MOD
