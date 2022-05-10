#pragma once

// PURPOSE: Hash CStr/Slice by different algorithms
//
// CLASSES:
//  Benchmark::xxhash_xx3_64bits: Use xxhash algo via XXH3_64bits API

#include <benchmark_cstr.h>
#include <benchmark_slice.h>

#include <iostream>

#include <city.h>
#include <t1ha.h>
#include <xxhash.h>

namespace Benchmark {

// +------------------+--------------------------------------------------+
// | Hash Algorithm   | Variation                                        |
// +------------------+--------------------------------------------------+
// | xxhash           | xx3_64bits: Canonical 64-bit                     |
// +------------------+--------------------------------------------------+
struct char_slice_xxhash_xx3_64bits {
  std::size_t operator()(const Slice<char>& key) const;
};

inline
std::size_t char_slice_xxhash_xx3_64bits::operator()(const Slice<char>& key) const {
  return XXH3_64bits(key.data(), key.size());
}

// +------------------+--------------------------------------------------+
// | Hash Algorithm   | Variation                                        |
// +------------------+--------------------------------------------------+
// | t1ha             | t1ha() with seed of 0                            |
// +------------------+--------------------------------------------------+
struct char_slice_t1ha {
  std::size_t operator()(const Slice<char>& key) const;
};

inline
std::size_t char_slice_t1ha::operator()(const Slice<char>& key) const {
  return t1ha(key.data(), key.size(), 0);
}

// +------------------+--------------------------------------------------+
// | Hash Algorithm   | Variation                                        |
// +------------------+--------------------------------------------------+
// | cityhash         | CityHash64                                       |
// +------------------+--------------------------------------------------+
struct char_slice_city_cityhash64 {
  std::size_t operator()(const Slice<char>& key) const;
};

inline
std::size_t char_slice_city_cityhash64::operator()(const Slice<char>& key) const {
  return CityHash64(key.data(), key.size());
}

} // namespace Benchmark
