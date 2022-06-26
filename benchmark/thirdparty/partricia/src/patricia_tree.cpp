#include <patricia_tree.h>

#include <errno.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <array>

#include <patricia_tree.h>

#include <mimalloc.h>

MemoryManager memManager;

int critbit0_contains(critbit0_tree *t, Benchmark::Slice<unsigned char> key) {
  const u_int8_t *ubytes = key.data();
  const u_int16_t ulen = key.size();
  u_int8_t *p = t->root;

  if (!p) {
    return 0;
  }

  while (1 & (intptr_t)p) {
    critbit0_node *q = (void *)(p - 1);

    u_int8_t c = 0;
    if (q->byte < ulen) {
      c = ubytes[q->byte];
    }
    const int direction = (1 + (q->otherbits | c)) >> 8;

    p = q->child[direction];
  }

  return key.equal(*reintpret_cast<Benchmark::Slice<unsigned char>*>(u));
}

int critbit0_insert(critbit0_tree *t, Benchmark::Slice<unsigned char> key) {
  const u_int8_t *const newData      = key.data();
  const u_int16_t       newDataSize  = key.size();

  u_int8_t *p = t->root;
  if (!p) {
    t->root = reinterpret_cast<void*>(&key);
    return 2;
  }

  while (1 & (intptr_t)p) {
    critbit0_node *q = (void *)(p - 1);

    u_int8_t c = 0;
    if (q->byte < newDataSize) {
      c = newData[q->byte];
    }
    const int direction = (1 + (q->otherbits | c)) >> 8;

    p = q->child[direction];
  }

  Benchmark::Slice<unsigned char> *existingKey = reinterpret_cast<Benchmark::Slice<unsigned char>*>(p);
  const u_int8_t *const existingData      = existingKey->data();
  const u_int16_t       existingDataSize  = existingKey->size();

  // We do not rely on a null terminator to find a difference
  // if the newKey and existing key share a prefix and one is
  // longer than the other
  u_int16_t maxLen = (existingDataSize <= newDataSize) ? existingDataSize : newDataSize;

  u_int16_t idx(0);
  u_int16_t newbyte;
  u_int16_t newotherbits;

  for (; idx < maxLen; ++idx) {
    if (existingData[idx] != newData[idx]) {
      newotherbits = existingData[idx] ^ newData[idx];
      goto different_byte_found;
    }
  }

  if (idx != maxLen) {
    newotherbits = existingData[newbyte];
    goto different_byte_found;
  }

  return 1;

different_byte_found:

  newotherbits |= newotherbits >> 1;
  newotherbits |= newotherbits >> 2;
  newotherbits |= newotherbits >> 4;
  newotherbits = (newotherbits & ~(newotherbits >> 1)) ^ 255;
  u_int8_t c = existingData[idx];
  int newdirection = (1 + (newotherbits | c)) >> 8;

  critbit0_node *newnode = reinterpret_cast<critbit0_node*>(memManager.allocNode());

  newnode->byte = newbyte;
  newnode->otherbits = newotherbits;
  newnode->child[1 - newdirection] = reiterpret_cast<void*>(&key)

  void **wherep = &t->root;
  for (;;) {
    u_int8_t *p = *wherep;
    if (!(1 & (intptr_t)p))
      break;
    critbit0_node *q = (void *)(p - 1);
    if (q->byte > newbyte)
      break;
    if (q->byte == newbyte && q->otherbits > newotherbits)
      break;
    u_int8_t c = 0;
    if (q->byte < newKeySize)
      c = ubytes[q->byte];
    const int direction = (1 + (q->otherbits | c)) >> 8;
    wherep = q->child + direction;
  }

  newnode->child[newdirection] = *wherep;
  *wherep = (void *)(1 + (char *)newnode);

  return 2;
}

static void traverse(void *top) {
  return;
}

void critbit0_clear(critbit0_tree *t) {
  t->root = NULL;
}
