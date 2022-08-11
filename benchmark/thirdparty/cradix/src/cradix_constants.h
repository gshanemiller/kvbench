#pragma once

#include <sys/types.h>

namespace CRadix {

static_assert(sizeof(int32_t)==4);
static_assert(sizeof(u_int32_t)==4);
static_assert(sizeof(int64_t)==8);
static_assert(sizeof(u_int64_t)==8);

enum {
  e_OK = 0,
  e_EXISTS = 1,
  e_NOT_FOUND = 2,
  e_MEMORY_ERROR = 3,
};

const u_int32_t k_MAX_CHILDREN = 256;
const u_int32_t k_NODE256_IS_DEAD = 0x1000000U;   // bit-24

const u_int64_t k_MEMMANAGER_MIN_ALIGN  = 4;
const u_int64_t k_MEMMANAGER_MIN_OFFSET = 8;
const u_int64_t k_MEMMANAGER_MIN_MEMORY = 1024;
const u_int32_t k_MEMMANAGER_DEFAULT_CAPACITY = 4;
const u_int64_t k_MEMMANAGER_MAX_MEMORY = 0x100000000UL;
const u_int64_t k_MEMMANAGER_DEADMEMORY_RESERVE = 65536;

const u_int32_t k_NODE256_IS_LEAF = 0x01;
const u_int32_t k_NODE256_IS_TERMINAL = 0x02;
const u_int32_t k_NODE256_ANY_TAG = 0x03;
const u_int32_t k_NODE256_NO_TAG_MASK = 0xFFFFFFFC;
const u_int32_t k_NODE256_CLR_LEAF_MASK = 0xFFFFFFFE;
const u_int32_t k_NODE256_CLR_TERMINAL_MASK = 0xFFFFFFFD;

} // namespace CRadix
