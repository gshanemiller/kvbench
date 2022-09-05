#pragma once

#include <sys/types.h>
#include <iostream>

namespace RingBuffer {

struct alignas(64) Op {
  // DATA
  u_int64_t           d_op;
  u_int64_t           d_arg0;
  u_int64_t           d_arg1;
  u_int64_t           d_ret;

  // CREATORS
  Op() = default;
    // Default construtor

  Op(u_int64_t op, u_int64_t arg0);
    // Create Op with specified 'op, arg0'

  Op(u_int64_t op, u_int64_t arg0, u_int64_t arg1);
    // Create Op with specified 'op, arg0, arg1'

  Op(const Op& other);
    // Copy constructor

  ~Op() = default;
    // Destory this object

  // MANIPULATORS
  Op& operator=(const Op& rhs);
    // Create and return a copy of specified 'rhs'

  // ASPECTS
  std::ostream& print(std::ostream& stream) const;
    // Pretty print into specified 'stream' a human readable dump of this object
};

// INLINE DEFINITIONS
// CREATORS
inline
Op::Op(u_int64_t op, u_int64_t arg0)
: d_op(op)
, d_arg0(arg0)
{
}

inline
Op::Op(u_int64_t op, u_int64_t arg0, u_int64_t arg1)
: d_op(op)
, d_arg0(arg0)
, d_arg1(arg1)
{
}

// MANIPULATORS
inline
Op& Op::operator=(const Op& rhs) {
  d_op    = rhs.d_op;
  d_arg0  = rhs.d_arg0;
  d_arg1  = rhs.d_arg1;
  d_ret   = rhs.d_ret;
  return *this;
}

// ASPECTS
inline
std::ostream& Op::print(std::ostream& stream) const {
  stream << "op: "    << d_op
         << " arg0: " << d_arg0
         << " arg1: " << d_arg1
         << " ret:  " << d_ret
         << std::endl;
  return stream;
}

};
