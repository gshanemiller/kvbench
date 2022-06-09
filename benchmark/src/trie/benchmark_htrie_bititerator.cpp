#include <trie/benchmark_htrie_bititerator.h>

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
