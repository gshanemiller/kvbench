#include <art.h>
#include <gtest/gtest.h>

static const struct {                                                                                                   
  int           d_lineNum;  // source line number                                                                       
  u_int16_t     d_size;     // size of string in bytes                                                                  
  u_int8_t      d_data[16]; // up to 16 bytes of data                                                                   
} VALUES[] = {                                                                                                          
  //line      size data                                                                                                 
  //----      ---- ----------------------------                                                                         
  { __LINE__,   8,  {'P', 'r', 'o', 'j', 'e', 'c', 't', 0 }},                                                           
  { __LINE__,  10,  {'G', 'u', 't', 'e', 'n', 'b', 'e', 'r', 'g', 0}},                                                  
  { __LINE__,   6,  {'E', 'B', 'o', 'o', 'k', 0}},                                                                      
  { __LINE__,   3,  {'o', 'f', 0}},                                                                                     
};  

extern "C" {
  int test_art_callback(void *data, const unsigned char *key, uint32_t key_len, void *value) {
    printf("got: data %p key %p key '%s' key_len: %u\n", data, key, key, key_len);
    return 0;
  }
}


TEST(art, construct) {
  art_tree artTrie;
  art_tree_init(&artTrie);                                                                                        
  art_insert(&artTrie, VALUES[3].d_data, VALUES[3].d_size, (void*)VALUES[3].d_data);
  art_insert(&artTrie, VALUES[0].d_data, VALUES[0].d_size, (void*)VALUES[0].d_data);
  art_insert(&artTrie, VALUES[1].d_data, VALUES[1].d_size, (void*)VALUES[1].d_data);
  art_insert(&artTrie, VALUES[2].d_data, VALUES[2].d_size, (void*)VALUES[2].d_data);
  art_iter(&artTrie, test_art_callback, 0);
}

