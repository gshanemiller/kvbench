#pragma once

// PURPOSE: Word scanner/iterator
//
// CLASSES:
//  Benchmark::TextScan: Given a file pre-loaded in memory in 'bin-text' format iterate through words

#include <benchmark_loadfile.h>
#include <benchmark_slice.h>

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

  unsigned count() const;
    // Return number of scanned words found so far

  unsigned available() const;
    // Return number of words available in file loaded in memory

  // MANIPULATORS
  void next(Slice<T>& value);
    // Assign to 'value' the next word in file provided at construction time
    // The behavior is defined provided '!eof()'.

  void next(Slice<unsigned T>& value);
    // Assign to 'value' the next word in file provided at construction time
    // The behavior is defined provided '!eof()'.

  void reset();
    // Reset internal state to point to the beginning of file.

  TextScan& operator=(const TextScan& rhs) = delete;
    // Assignment operator not provided
};

// INLINE DEFINITIONS
// CREATORS
template<T>
inline
TextScan::TextScan(const LoadFile& file)
: d_file(file)
{
  reset();
}

// ACCESSORS
template<T>
inline
bool TextScan::eof() const {
  return (d_index>=d_available);
}

template<T>
inline
unsigned int TextScan::count() const {
  return d_index;
}

template<T>
inline
unsigned int TextScan::available() const {
  return d_available;
}

// MANIPULATORS
template<T>
inline
void TextScan::next(Slice<T>& word) {
  assert(!eof());

  ++d_index;

  unsigned int *i = reinterpret_cast<unsigned int*>(d_ptr);
  word.reset(d_ptr+sizeof(unsigned int), (*i)&0xffff);

  d_ptr += sizeof(unsigned int) + ((*i)&0xffff) + ((*i)>>16);
}

template<T>
inline
void TextScan::next(Slice<unsigned char>& word) {
  assert(!eof());

  ++d_index;

  unsigned int *i = reinterpret_cast<unsigned int*>(d_ptr);

//printf("TextScan::next: making word %u of %u with size %u padding %u at %p\n",
//  d_index-1, d_available, (*i)&0xffff, (*i)>>16, reinterpret_cast<unsigned char*>(d_ptr));
  word.reset(reinterpret_cast<unsigned char*>(d_ptr)+sizeof(unsigned int), (*i)&0xffff);

  d_ptr += sizeof(unsigned int) + ((*i)&0xffff) + ((*i)>>16);
}

template<T>
inline
void TextScan::reset() {
  d_ptr = d_file.data();
  d_end = d_file.data()+d_file.fileSize();
  d_available = 0;
  d_index = 0;
  if (static_cast<unsigned long>(d_end-d_ptr)>=sizeof(unsigned int)) {
    unsigned int *i = reinterpret_cast<unsigned int*>(d_ptr);
    d_available = *i;
    d_ptr += sizeof(unsigned int);
  }
}

} // namespace Benchmark
