#pragma once 

#include <sys/types.h>

namespace CRadix {

struct Node256;

union PtrHelper {
  u_int64_t          d_val;
  Node256           *d_obj;
  u_int8_t          *d_ptr;

  PtrHelper()
  : d_val(0)
  {
  }

  explicit PtrHelper(u_int64_t val)
  : d_val(val)
  {
  }

  explicit PtrHelper(Node256 *ptr)
  : d_obj(ptr)
  {
  }

  explicit PtrHelper(u_int8_t *ptr)
  : d_ptr(ptr)
  {
  }
};

} // namespace CRadix
