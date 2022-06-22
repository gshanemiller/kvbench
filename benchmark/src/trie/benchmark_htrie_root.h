#pragma once

// PURPOSE: Trie over byte sequences
//          
// CLASSES:
//          Benchmark::HTrie::Root: Trie and client API
//
// IMPLEMENTATION CLASSES:
//          
//          Benchmark::HTrie::Node: Children nodes of Root

#include <trie/benchmark_htrie_bititerator.h>

#include <iostream>

namespace Benchmark {
namespace HTrie {

struct Node {
  // DATA
  bool d_bit;               // bit-value of this node
  Node *d_zero;             // bit-0 left tree
  Node *d_one;              // bit-1 right tree

  // CREATORS
  Node() = delete;
    // Default constructor not provided

  Node(bool value);
    // Create a node with specified 'value' having no children

  ~Node() = default;
    // Destroy this object
  
  Node(const Node& other) = delete;
    // Copy constructor not provided

  // MANIPULATORS
  const Node& operator=(const Node& rhs) = delete;
    // Assignment operator not provided
};

// INLINE DEFINITIONS
inline
Node::Node(bool value)
: d_bit(value)
, d_zero(0)
, d_one(0)
{
}

class Root: public Node {
public:
  // CREATORS
  Root();
    // Default constructor

  Root(const Root& other) = delete;
    // Copy constructor not provided

  ~Root() = default;
    // Destroy this object

  // ACCESSORS
  htrie_size height() const;
    // Return the maximum height of this trie

  bool find(BitIterator& key) const;
    // Return true if specified 'key' found in trie else false

  // MANIPULATORS
  bool insert(BitIterator& key);
    // Return true if specified 'key' was inserted into trie and false if 'key'
    // already exists

  bool remove(BitIterator& key);
    // Return true if specified 'key' was found and deleted from trie and false
    // if 'key' was not found
 
  const Root& operator=(const Root& rhs) = delete;
    // Assignment operator not provided

private:
  // PRIVATE MANIPULATORS
  htrie_size printHelper(std::ostream& stream, Node *parent, htrie_size itemNo) const;
    // Print helper

public:
  // ASPECTS
  std::ostream& print(std::ostream& stream) const;
    // Pretty-print this object in AT&T dot-format to specified stream
};

// FREE OPERATORS
std::ostream& operator<<(std::ostream& stream, const Root& object);
    // Print into specified 'stream' an AT&T dot representation 'object' returning 'stream'

// INLINE DEFINITIONS

// CREATORS
inline
Root::Root()
: Node(0)
{
}

// ACCESSORS
inline
htrie_size Root::height() const {
  return 0;
}

// ASPECTS
inline
std::ostream& operator<<(std::ostream& stream, const Root& object) {
  return object.print(stream);
}

} // namespace HTrie
} // namespace Benchmark
