#pragma once

#include <assert.h>
#include <sys/types.h>

namespace CRadix {

struct IterState {
  u_int32_t d_node;
  u_int16_t d_index;
  u_int16_t d_depth;

  // CREATORS
  IterState() = delete;
    // Default constructor not provided

  IterState(const IterState& other) = default;
    // Copy constructor

  explicit IterState(u_int32_t node, u_int16_t index, u_int16_t depth);
    // Construct IterState with specified arguments

  // MANIUPULATORS
  const IterState& operator=(const IterState& rhs) = delete;
    // Assignment operator not provided
};

// INLINE DEFINITIONS
// CREATORS

inline
IterState::IterState(u_int32_t node, u_int16_t index, u_int16_t depth)
: d_node(node)
, d_index(index)
, d_depth(depth)
{
  assert(node!=0);
}

} // namespace CRadix
