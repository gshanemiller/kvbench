#pragma once

// PURPOSE: Typedefs for HTrie
          
#include <iostream>

namespace Benchmark {
namespace HTrie     {

// TYPEDEFS
typedef unsigned int htrie_size;
typedef unsigned int htrie_index;

typedef unsigned char   htrie_byte;
typedef unsigned char   htrie_uint8;
typedef unsigned short  htrie_uint16;
typedef unsigned int    htrie_uint32;
typedef unsigned long   htrie_uint64;

// STATIC ASSERTS                                                                                                     
static_assert(sizeof(htrie_size)==4);
static_assert(sizeof(htrie_index)==4);

static_assert(sizeof(htrie_byte)==1);
static_assert(sizeof(htrie_uint8)==1);
static_assert(sizeof(htrie_uint16)==2);
static_assert(sizeof(htrie_uint32)==4);
static_assert(sizeof(htrie_uint64)==8);

} // namespace HTrie
} // namespace Benchmark
