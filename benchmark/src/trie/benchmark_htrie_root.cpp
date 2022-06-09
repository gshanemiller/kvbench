#include <trie/benchmark_htrie_root.h>

bool Benchmark::HTrie::Root::find(BitIterator& key) const {                                                                
  return false;                                                                                                         
}                                                                                                                       
                                                                                                                        
bool Benchmark::HTrie::Root::insert(BitIterator& key) {
  Benchmark::HTrie::Node *parent = this;
  
  for (key.begin(); !key.end(); key.next()) {
    if (key.value()) {
      if (parent->d_one==0) {
        parent->d_one = new Node(true);
      }
      parent=parent->d_one;
    } else if (parent->d_zero==0) { 
        parent->d_zero = new Node(false);
        parent=parent->d_zero;
    } else {
        parent=parent->d_zero;
    }
  }

  return false;                                                                                                         
}                                                                                                                       
                                                                                                                        
bool Benchmark::HTrie::Root::remove(BitIterator& key) {                                                                    
  return false;                                                                                                         
} 

Benchmark::HTrie::htrie_size Benchmark::HTrie::Root::printHelper(std::ostream& stream,
  Benchmark::HTrie::Node *parent, Benchmark::HTrie::htrie_size itemNo) const {
  const Benchmark::HTrie::htrie_size parentItemNo(itemNo);
  if (parent->d_zero) {
    stream << "  node_" << (itemNo+1) << " [label=\"0\"];" << std::endl;
    stream << "  node_" << parentItemNo << " -> node_" << (itemNo+1) << std::endl;
    ++itemNo;
    itemNo = printHelper(stream, parent->d_zero, itemNo);
  }
  if (parent->d_one) {
    stream << "  node_" << (itemNo+1) << " [label=\"1\"];" << std::endl;
    stream << "  node_" << parentItemNo << " -> node_" << (itemNo+1) << std::endl;
    itemNo = printHelper(stream, parent->d_one, ++itemNo);
  }
  return itemNo;
}

std::ostream& Benchmark::HTrie::Root::print(std::ostream& stream) const {
  stream << "digraph HTrie {"          << std::endl;
  Benchmark::HTrie::Node *parent = (Benchmark::HTrie::Node*)(this);
  stream << "  node_0 [shape=diamond label=\"root\"];" << std::endl;
  printHelper(stream, parent, 0);
  stream << "}"                         << std::endl;
  return stream;
}
