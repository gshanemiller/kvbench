#include <patricia_tree.h>

#include <errno.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

Patricia::MemoryManager memManager;

int Patricia::findKey(Patricia::Tree *t, Benchmark::UKey key) {
  assert(t);
  assert(key.data());
  assert(key.size());

  if (!t->root) {
    return Patricia::Errno::e_NOT_FOUND;
  }

  const u_int8_t *const keyData      = key.data();
  const u_int16_t       keyDataSize  = key.size();

  intptr_t p = reinterpret_cast<intptr_t>(t->root);
  while (1 & p) {
    Patricia::InternalNode *q = reinterpret_cast<Patricia::InternalNode*>(p-1);

    u_int8_t c = 0;
    if (q->diffIndex < keyDataSize) {
      c = keyData[q->diffIndex];
    }
    const int direction = (1 + (q->diffMask | c)) >> 8;

    p = reinterpret_cast<intptr_t>(q->child[direction]);
  }

//return 0==key.equal(*reintpret_cast<Benchmark::UKey*>(p))
//  ? Patricia::Errno::e_OK
//  : Patricia::Errno::e_NOT_FOUND;
  return Patricia::Errno::e_OK;
}

int Patricia::insertKey(Patricia::Tree *t, Benchmark::UKey key) {
  assert(t);
  assert(key.data());
  assert(key.size());

  if (!t->root) {
    t->root = reinterpret_cast<void*>(&key);
    return Patricia::Errno::e_OK;
  }

  const u_int8_t *const newData      = key.data();
  const u_int16_t       newDataSize  = key.size();

  intptr_t p = reinterpret_cast<intptr_t>(t->root);
  while (1 & p) {
    Patricia::InternalNode *q = reinterpret_cast<Patricia::InternalNode*>(p-1);

    u_int8_t c = 0;
    if (q->diffIndex < newDataSize) {
      c = newData[q->diffIndex];
    }
    const int direction = (1 + (q->diffMask | c)) >> 8;

    p = reinterpret_cast<intptr_t>(q->child[direction]);
  }

  Benchmark::UKey *existingKey = reinterpret_cast<Benchmark::UKey*>(p);
  const u_int8_t *const existingData      = existingKey->data();
  const u_int16_t       existingDataSize  = existingKey->size();

  // Ensure we do not read off the end of either key
  u_int16_t maxSize = (existingDataSize <= newDataSize) ? existingDataSize : newDataSize;

  u_int16_t idx(0);
  u_int16_t newDiffMask;

  for (; idx < maxSize; ++idx) {
    if (existingData[idx] != newData[idx]) {
      newDiffMask = existingData[idx] ^ newData[idx];
      goto different_byte_found;
    }
  }

  if (idx != maxSize) {
    newDiffMask = existingData[idx];
    goto different_byte_found;
  }

  // Key already exists
  return Patricia::Errno::e_EXISTS;

different_byte_found:

  newDiffMask |= newDiffMask >> 1;
  newDiffMask |= newDiffMask >> 2;
  newDiffMask |= newDiffMask >> 4;
  newDiffMask = (newDiffMask & ~(newDiffMask >> 1)) ^ 255;
  assert(idx<existingDataSize);
  u_int8_t c = existingData[idx];
  int newDirection = (1 + (newDiffMask | c)) >> 8;

  Patricia::InternalNode *newNode = memManager.allocInternalNode();

  newNode->diffIndex = idx;
  newNode->diffMask = newDiffMask;
  newNode->child[1 - newDirection] = reinterpret_cast<void*>(&key);

  void **wherep = reinterpret_cast<void**>(&t->root);
  for (;;) {
    intptr_t p = reinterpret_cast<intptr_t>(*wherep);
    if (!(1 & p))
      break;
    Patricia::InternalNode *q = reinterpret_cast< Patricia::InternalNode*>(p-1);
    if (q->diffIndex > idx)
      break;
    if (q->diffIndex == idx && q->diffMask > newDiffMask)
      break;
    u_int8_t c = 0;
    if (q->diffIndex < existingDataSize)
      c = existingData[q->diffIndex];
    const int direction = (1 + (q->diffMask | c)) >> 8;
    wherep = q->child + direction;
  }

  newNode->child[newDirection] = *wherep;
  *wherep = reinterpret_cast<void*>(1 + (char *)newNode);

  return Patricia::Errno::e_OK;
}
