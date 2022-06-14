#pragma once

// PURPOSE: Typedefs for HTrie
          
#include <iostream>

namespace Benchmark {
namespace HTrie     {

// TYPEDEFS
typedef unsigned int htrie_size;
typedef unsigned int htrie_index;
typedef unsigned char htrie_byte;
typedef unsigned short htrie_len;
typedef unsigned long htrie_word;

// STATIC ASSERTS                                                                                                     
static_assert(sizeof(htrie_byte)==1);
static_assert(sizeof(htrie_len)==2);
static_assert(sizeof(htrie_size)==4);
static_assert(sizeof(htrie_index)==4);
static_assert(sizeof(htrie_word)==8);

} // namespace HTrie
} // namespace Benchmark
