#define ATOMIC_CLAUSE capture
#define ATOMIC_REAL
#define ATOMIC_OPTYPE EXPR_PLUS_X
#define ATOMIC_ASSIGN_FIRST
#define ATOMIC_INIT 0.0_8
#define ATOMIC_SIZE 10
#ifndef T1
!T1:construct-independent,atomic,V:2.0-2.7
#include "atomic_template.Fh"
#endif
