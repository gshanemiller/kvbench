#pragma once

#include <sys/types.h>                                                                                                  

#ifdef __cplusplus
extern "C" {
#endif

void *art_calloc(u_int64_t size);
void *art_malloc(u_int64_t size);
void art_free(void *ptr);
void art_print_memory(void);

#ifdef __cplusplus
}
#endif
