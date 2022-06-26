#include <patricia_tree.h>

#include <errno.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

static MemoryManager memManager;

int Patricia::find(Patricia::Tree *t, Benchmark::Key key) {
  u_int8_t *p = t->root;
  if (!p) {
    return Patricia::Errno::e_NOT_FOUND;
  }

  const u_int8_t *const keyData      = key.data();
  const u_int16_t       keyDataSize  = key.size();

  while (1 & (intptr_t)p) {
    Patricia::InternalNode *q = reinterpret_cast<void *>(p-1);

    u_int8_t c = 0;
    if (q->diffIndex < keyDataSize) {
      c = keyData[q->diffIndex];
    }
    const int direction = (1 + (q->diffMask | c)) >> 8;

    p = q->child[direction];
  }

  return 0=key.equal(*reintpret_cast<Benchmark::Key*>(p))
    ? Patricia::Errno::e_OK
    : Patricia::Errno::e_NOT_FOUND;
}

int Patricia::insert(Patricia::Tree *t, Benchmark::Slice<unsigned char> key) {
  const u_int8_t *const newData      = key.data();
  const u_int16_t       newDataSize  = key.size();

  u_int8_t *p = t->root;
  if (!p) {
    t->root = reinterpret_cast<void*>(&key);
    return Patricia::Errno::e_OK;
  }

  while (1 & (intptr_t)p) {
    Patricia::InternalNode *q = reinterpret_cast<void *>(p-1);

    u_int8_t c = 0;
    if (q->diffIndex < newDataSize) {
      c = newData[q->diffIndex];
    }
    const int direction = (1 + (q->diffMask | c)) >> 8;

    p = q->child[direction];
  }

  Benchmark::Slice<unsigned char> *existingKey = reinterpret_cast<Benchmark::Key*>(p);
  const u_int8_t *const existingData      = existingKey->data();
  const u_int16_t       existingDataSize  = existingKey->size();

  // Ensure we do not read off the end of either key
  u_int16_t maxSize = (existingDataSize <= newDataSize) ? existingDataSize : newDataSize;

  u_int16_t idx(0);
  u_int16_t newDiffIndex;
  u_int16_t newDiffMask;

  for (; idx < maxSize; ++idx) {
    if (existingData[idx] != newData[idx]) {
      newDiffMask = existingData[idx] ^ newData[idx];
      goto different_byte_found;
    }
  }

  if (idx != maxSize) {
    newDiffMask = existingData[newbyte];
    goto different_byte_found;
  }

  return Patricia::Errno::e_EXISTS;

different_byte_found:

  newDiffMask |= newotherbits >> 1;
  newDiffMask |= newotherbits >> 2;
  newDiffMask |= newotherbits >> 4;
  newDiffMask = (newDiffMask & ~(newDiffMask>> 1)) ^ 255;
  u_int8_t c = existingData[idx];
  int newDirection = (1 + (newDiffMask | c)) >> 8;

  Patricia::InternalNode *newNode = reinterpret_cast<Patricia::InternalNode*>(memManager.allocNode());

  newNode->diffIndex = idx;
  newNode->diffMask = newDiffMask;
  newNode->child[1 - newdirection] = reiterpret_cast<void*>(&key)
  newNode->child[!(1 - newdirection)] = 0;

  void **wherep = &t->root;
  for (;;) {
    u_int8_t *p = *wherep;
    if (!(1 & (intptr_t)p))
      break;
    Patricia::InternalNode *q = reinterpret_cast<void *>(p-1);
    if (q->diffIndex > idx)
      break;
    if (q->diffIndex == idx && q->diffMask > newDiffMask)
      break;
    u_int8_t c = 0;
    if (q->diffInxex < idx)
      c = ubytes[q->byte];
    const int direction = (1 + (q->diffMask | c)) >> 8;
    wherep = q->child + direction;
  }

  newNode->child[newdirection] = *wherep;
  *wherep = (void *)(1 + (char *)newnode);

  return Patricia::Errno::e_OK;
}
