#pragma once

// PURPOSE: Compilation and code generation options

#ifdef __GNUC__
#define HTRIE_ALWAYS_INLINE __attribute__((always_inline))
#else
#define HTRIE_ALWAYS_INLINE inline
#endif
