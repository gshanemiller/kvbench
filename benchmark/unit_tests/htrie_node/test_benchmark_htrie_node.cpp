#include <trie/benchmark_htrie_node.h>
#include <gtest/gtest.h>
#include <random>

static std::mt19937_64 rng;
static bool seedrng=false;
static std::uniform_int_distribution<uint64_t> distribution(0, 0xffffffffffffffff);

static void seedRng() {
  if (seedrng) {
    auto seed = std::random_device{}();
    rng.seed(seed);
  }
}


