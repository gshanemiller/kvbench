#pragma once

#include <sys/types.h>                                                                                                  
#include <art.h>

#ifdef __cplusplus
extern "C" {
#endif

void *art_calloc(u_int64_t size);
void *art_malloc(u_int64_t size);

void art_free_node(art_node*ptr);
void art_free_leaf(art_leaf*ptr);

void art_print_memory(void);

void art_mem_stats_reset(void);

#ifdef __cplusplus
}
#endif
