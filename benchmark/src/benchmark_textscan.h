#pragma once

// PURPOSE: Word scanner/iterator
//
// CLASSES:
//  Benchmark::TextScan: Given a file pre-loaded in memory in 'bin-text' format iterate through words

#include <benchmark_loadfile.h>
#include <benchmark_slice.h>

#include <vector>
#include <string>

#include <ctype.h>
#include <assert.h>

namespace Benchmark {

template<typename T>
class TextScan {
  // DATA
  const LoadFile& d_file;       // holds pointer to memory array
  char *          d_ptr;        // current memory position in memory array
  char *          d_end;        // end of memory array
  unsigned int    d_available;  // word count in loaded file
  unsigned int    d_index;      // current word in [0, d_available)

public:
  // CREATORS
  explicit TextScan(const LoadFile& file);
    // Create a TextScan object which will scan over the text in specified 'file'. The behavior is defined
    // provided 'file.load()' was error-free.

  TextScan(const TextScan& other) = delete;
    // Copy constructor not provided

  ~TextScan() = default;
    // Destroy this object.

  // ACCESSORS
  bool eof() const;
    // Return true if EOF reached.

  unsigned int index() const;
    // Return number of scanned words found so far

  unsigned int available() const;
    // Return number of words available in file loaded in memory

  // MANIPULATORS
  void next(Slice<T>& value);
    // Assign to 'value' the next word in file provided at construction time
    // The behavior is defined provided '!eof()'.

  void reset();
    // Reset internal state to point to the beginning of file.

  int exportAsVector(std::vector<std::string>& data);
    // Return if from current 'index()' all remaining data is pushed one 'std::string*' per word into specified 'data'
    // and non-zero otherwise. You must call 'reset()' after call to restart scanning. Note 'data' is cleared first.

  TextScan& operator=(const TextScan& rhs) = delete;
    // Assignment operator not provided
};

// INLINE DEFINITIONS
// CREATORS
template<class T>
inline
TextScan<T>::TextScan(const LoadFile& file)
: d_file(file)
{
  reset();
}

// ACCESSORS
template<class T>
inline
bool TextScan<T>::eof() const {
  return (d_index>=d_available);
}

template<class T>
inline
unsigned int TextScan<T>::index() const {
  return d_index;
}

template<class T>
inline
unsigned int TextScan<T>::available() const {
  return d_available;
}

// MANIPULATORS
template<class T>
inline
void TextScan<T>::next(Slice<T>& word) {
  assert(!eof());

  ++d_index;

  unsigned int *i = reinterpret_cast<unsigned int*>(d_ptr);
  word.reset((const T*)(d_ptr+sizeof(unsigned int)), (*i)&0xffff);

  d_ptr += sizeof(unsigned int) + ((*i)&0xffff)*sizeof(T);
}

template<class T>
inline
void TextScan<T>::reset() {
  d_ptr = d_file.data();
  d_end = d_file.data()+d_file.fileSize();
  d_index = 0;
  d_available = 0;
  if (static_cast<unsigned long>(d_end-d_ptr)>=sizeof(unsigned int)) {
    unsigned int *i = reinterpret_cast<unsigned int*>(d_ptr);
    d_available = *i;
    d_ptr += sizeof(unsigned int);
  }
}

template<class T>
inline
int TextScan<T>::exportAsVector(std::vector<std::string>& data) {
  data.clear();
  Slice<T> word;
  while (!eof()) {
    next(word);
    data.emplace_back(std::string(word.const_data(), word.size()));
  }

  return 0;
}

} // namespace Benchmark
