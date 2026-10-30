MODULE DECLARE_COPYIN_MOD
  INTEGER, DIMENSION(0:9) :: fixed_size_array
  INTEGER :: scalar
  !$acc declare copyin(fixed_size_array, scalar)
END MODULE DECLARE_COPYIN_MOD
