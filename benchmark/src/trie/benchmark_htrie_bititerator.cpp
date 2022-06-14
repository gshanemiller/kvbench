#include <trie/benchmark_htrie_bititerator.h>

#include <assert.h>
#include <algorithm>

std::ostream& Benchmark::HTrie::BitIterator::print(std::ostream& stream) const {
  stream << "\"BitIterator\": {"                              << std::endl;
  stream << "  \"data\"     : " << (void*)(d_data)            << std::endl;
  stream << "  \"sizeBits\" : " << bits()                     << std::endl;
  stream << "  \"bitIndex\" : " << bitIndex()                 << std::endl;
  stream << "  \"value\"    : " << (int)value()               << std::endl;
  stream << "}"                                               << std::endl;
  return stream;
}

std::ostream& Benchmark::HTrie::BitIterator::dump(std::ostream& stream) const {
  stream << "\"BitIterator\": {"                              << std::endl;
  stream << "  \"data\"     : " << (void*)(d_data)            << std::endl;
  stream << "  \"sizeBits\" : " << bits()                     << std::endl;
  stream << "  \"value\"    : '";

  Benchmark::HTrie::BitIterator tmp(*this);
  for (tmp.begin(); !tmp.end(); tmp.next()) {
    if (tmp.bitIndex()&&(tmp.bitIndex()&7)==0) {
      stream << '|';
    }
    stream << (int)tmp.value();
  }
  if ((tmp.bitIndex()&7)==0) {
    stream << '|';
  }

  stream << "'" << std::endl;
  stream << "}"                                               << std::endl;
  return stream;
}

Benchmark::HTrie::htrie_index Benchmark::HTrie::BitIterator::longestCommonPrefix(const BitIterator &rhs) {
  // 'range-1' represents the maximum bit relative to current position in this,
  // rhs in which a bit difference can occur 
  htrie_size range = std::min(d_size-d_index, rhs.d_size-rhs.d_index);
  return range;
}
