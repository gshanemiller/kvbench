#pragma once

#include <assert.h>
#include <sys/types.h>

namespace CRadix {

struct TreeIterState {
  u_int32_t d_node;
  u_int16_t d_index;
  u_int16_t d_depth;

  // CREATORS
  TreeIterState() = delete;
    // Default constructor not provided

  TreeIterState(const TreeIterState& other) = delete;
    // Copy constructor not provided

  explicit TreeIterState(u_int32_t node, u_int16_t index, u_int16_t depth);
    // Construct TreeIterState with specified arguments

  // MANIUPULATORS
  const TreeIterState& operator=(const TreeIterState& rhs) = delete;
    // Assignment operator not provided
};

// INLINE DEFINITIONS
// CREATORS

inline
TreeIterState::TreeIterState(u_int32_t node, u_int16_t index, u_int16_t depth)
: d_node(node)
, d_index(index)
, d_depth(depth)
{
  assert(node!=0);
}

} // namespace CRadix
