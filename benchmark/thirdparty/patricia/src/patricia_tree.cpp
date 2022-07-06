#include <patricia_tree.h>

#include <errno.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stack>

Patricia::MemoryManager memManager;

int Patricia::findKey(Patricia::Tree *t, Benchmark::UKey key) {
  assert(t);
  assert(key.data());
  assert(key.size());

  if (!t->root) {
    return Patricia::Errno::e_NOT_FOUND;
  }

  const u_int8_t *const keyData      = key.data();
  const u_int16_t       keyDataSize  = key.size() - 1;

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

  return key.equal(reinterpret_cast<void*>(p)) ? Patricia::Errno::e_OK : Patricia::Errno::e_NOT_FOUND;
}

int Patricia::insertKey(Patricia::Tree *t, Benchmark::UKey key) {
  assert(t);
  assert(key.data());
  assert(key.size());

  if (!t->root) {
    t->root = static_cast<void*>(key);
    return Patricia::Errno::e_OK;
  }

  const u_int8_t *const newData      = key.data();
  const u_int16_t       newDataSize  = key.size() - 1;

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

  Benchmark::UKey existingKey(reinterpret_cast<void*>(p));
  const u_int8_t *const existingData      = existingKey.data();
  const u_int16_t       existingDataSize  = existingKey.size() - 1;

  u_int16_t idx(0);
  u_int16_t newDiffMask;

  for (; idx < newDataSize; ++idx) {
    if (existingData[idx] != newData[idx]) {
      newDiffMask = existingData[idx] ^ newData[idx];
      goto different_byte_found;
    }
  }

  if (existingData[idx] != 0) {
    // printf("idx is %u existingDataSize %u newDataSize %u\n", idx, existingDataSize, newDataSize);
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
  u_int8_t c = existingData[idx];
  int newDirection = (1 + (newDiffMask | c)) >> 8;

  Patricia::InternalNode *newNode = memManager.allocInternalNode();

  newNode->diffIndex = idx;
  newNode->diffMask = newDiffMask;
  newNode->child[1 - newDirection] = static_cast<void*>(key);

  void **wherep = reinterpret_cast<void**>(&t->root);
  for (;;) {
    intptr_t p = reinterpret_cast<intptr_t>(*wherep);
    if (!(1 & p))
      break;
    Patricia::InternalNode *q = reinterpret_cast<Patricia::InternalNode*>(p-1);
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

void Patricia::allKeysSorted(Tree *t, std::vector<Benchmark::UKey>& leaf) {
  leaf.clear();
  
  if (!t->root) {
    return;
  }

  intptr_t p = reinterpret_cast<intptr_t>(t->root);

  // There's only one leaf?
  if ((1&p)==0) {
    leaf.push_back(Benchmark::UKey(t->root));
    return;
  }

  std::stack<intptr_t> stack;
  Patricia::InternalNode *q(0);

  // In-order tree traversal
descend_left:
  while (p && (1 & p)) {
    stack.push(p);
    q = reinterpret_cast<Patricia::InternalNode*>(p-1);
    p = reinterpret_cast<intptr_t>(q->child[0]);
  }

  // Did escape loop because found leaf or left-child was 0?
  if (p) {
    assert((1&p)==0);
    leaf.push_back(Benchmark::UKey(reinterpret_cast<void*>(p)));
  }

  // More work to do?
  if (stack.empty()) {
    return;
  }

  p = stack.top();
  assert(p&1);
  stack.pop();

  q = reinterpret_cast<Patricia::InternalNode*>(p-1);
  p = reinterpret_cast<intptr_t>(q->child[1]);
  
  goto descend_left;
}

void Patricia::destroyTree(Patricia::Tree *tree) {
  assert(tree);
  
  return;
}
