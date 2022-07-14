#pragma once

#include <sys/types.h>

namespace Radix {

enum {
  k_MAX_CHILDREN256 = 256,
  k_IS_INNER_NODE = 1,
  k_IS_INNER_NODE256 = 2,
  k_IS_KEY_COMPRESSED = 4,
  k_IS_CHILDREN_COMPRESSED = 8,
  k_IS_LEAF_NODE = 0xff,
  e_OK = 0,
  e_EXISTS = 1,
  e_NOT_FOUND = 2,
  e_MEMORY_ERROR = 3,
};

} // namespace Radix
