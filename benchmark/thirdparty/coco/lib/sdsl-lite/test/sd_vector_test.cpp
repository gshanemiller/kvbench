#include "sdsl/sd_vector.hpp"
#include "sdsl/bit_vectors.hpp"
#include "gtest/gtest.h"

using namespace sdsl;
using namespace std;

namespace
{

const size_t BV_SIZE = 1000000;

std::vector<uint64_t>
generate_positions(size_t size, size_t inverse_density)
{
    std::mt19937_64 rng;
    std::uniform_int_distribution<uint64_t> distribution(0, inverse_density);
    auto dice = bind(distribution, rng);

    std::vector<uint64_t> result;
    for (size_t i = 0; i < size; ++i) {
        if (dice() == 0) {
            result.push_back(i);
        }
    }

    return result;
}

bit_vector
mark_positions(const std::vector<uint64_t>& positions, size_t size)
{
    bit_vector result(size);
    for (uint64_t pos : positions) { result[pos] = 1; }
    return result;
}

template<class T>
class sd_vector_test : public ::testing::Test { };

using testing::Types;

typedef Types< sd_vector<>, sd_vector<rrr_vector<63>> > Implementations;

TYPED_TEST_CASE(sd_vector_test, Implementations);

TYPED_TEST(sd_vector_test, iterator_constructor)
{
    std::vector<uint64_t> pos = generate_positions(BV_SIZE, 9);
    bit_vector bv = mark_positions(pos, BV_SIZE);
    TypeParam sdv(pos.begin(), pos.end());

    ASSERT_EQ(sdv.ones(), pos.size());
    for (size_t i = 0; i < bv.size(); i++) {
        ASSERT_EQ((bool)sdv[i],(bool)bv[i]);
    }
}

TYPED_TEST(sd_vector_test, builder_constructor)
{
    std::vector<uint64_t> pos = generate_positions(BV_SIZE, 9);
    bit_vector bv = mark_positions(pos, BV_SIZE);
    sd_vector_builder builder(BV_SIZE, pos.size());
    for (auto i : pos) {
        builder.set(i);
    }
    TypeParam sdv(builder);

    ASSERT_EQ(sdv.size(), BV_SIZE);
    ASSERT_EQ(sdv.ones(), pos.size());
    for (size_t i = 0; i < bv.size(); i++) {
        ASSERT_EQ((bool)sdv[i],(bool)bv[i]);
    }
}

TYPED_TEST(sd_vector_test, one_iterator)
{
    std::vector<uint64_t> pos = generate_positions(BV_SIZE, 9);
    sd_vector_builder builder(BV_SIZE, pos.size());
    for (auto i : pos) {
        builder.set(i);
    }
    TypeParam sdv(builder);

    // At end.
    {
        auto iter = sdv.one_end();
        ASSERT_EQ(iter->first, sdv.ones());
        ASSERT_EQ(iter->second, sdv.size());
    }

    // Iterate forward.
    size_t expected = 0;
    for (auto iter = sdv.one_begin(); iter != sdv.one_end(); ++iter) {
        ASSERT_EQ(iter->first, expected);
        ASSERT_EQ(iter->second, pos[expected]);
        expected++;
    }
    ASSERT_EQ(expected, pos.size());

    // Iterate backward.
    auto begin = sdv.one_begin();
    for (auto iter = sdv.one_end(); iter != begin;) {
        --iter; expected--;
        ASSERT_EQ(iter->first, expected);
        ASSERT_EQ(iter->second, pos[expected]);
    }
    ASSERT_EQ(expected, size_t(0));

    // Iterator-based select.
    ASSERT_EQ(sdv.select_iter(0), sdv.one_end());
    for (size_t i = 0; i < sdv.ones(); i++) {
        auto iter = sdv.select_iter(i + 1);
        ASSERT_EQ(iter->first, i);
        ASSERT_EQ(iter->second, pos[i]);
    }
    ASSERT_EQ(sdv.select_iter(sdv.ones() + 1), sdv.one_end());

    // Predecessor.
    pos.push_back(sdv.size());
    size_t start = 0;
    auto iter = sdv.one_end();
    for (size_t i = 0; i < pos.size(); i++) {
        for (size_t j = start; j < pos[i]; j++) {
            ASSERT_EQ(sdv.predecessor(j), iter);
        }
        if (i + 1 < pos.size()) {
            iter = sdv.predecessor(pos[i]);
            ASSERT_EQ(iter->first, i);
            ASSERT_EQ(iter->second, pos[i]);
            start = pos[i] + 1;
        }
    }
    ASSERT_EQ(sdv.predecessor(sdv.size()), iter);

    // Successor.
    start = 0;
    for (size_t i = 0; i < pos.size(); i++) {
        iter = sdv.successor(pos[i]);
        ASSERT_EQ(iter->first, i);
        ASSERT_EQ(iter->second, pos[i]);
        for (size_t j = start; j < pos[i]; j++) {
            ASSERT_EQ(sdv.successor(j), iter);
        }
        start = pos[i] + 1;
    }
    ASSERT_EQ(sdv.successor(sdv.size()), iter);
}

TYPED_TEST(sd_vector_test, multiset)
{
    std::vector<uint64_t> pos = { 123, 131, 131, 131, 347, 961 };
    sd_vector_builder builder(1024, pos.size(), true);
    for (auto i : pos) {
        builder.set(i);
    }
    TypeParam sdv(builder);
    TypeParam second(pos.begin(), pos.end());

    // Basic tests.
    ASSERT_EQ(sdv.ones(), pos.size());
    ASSERT_EQ(second.ones(), pos.size());

    // Iterate forward.
    size_t expected = 0;
    for (auto iter = sdv.one_begin(), second_iter = second.one_begin(); iter != sdv.one_end(); ++iter, ++second_iter) {
        ASSERT_EQ(iter->first, expected);
        ASSERT_EQ(iter->second, pos[expected]);
        ASSERT_EQ(*iter, *second_iter);
        expected++;
    }
    ASSERT_EQ(expected, pos.size());

    // Special cases.
    auto first_duplicate = sdv.select_iter(2);
    ASSERT_EQ(first_duplicate->first, uint64_t(1));
    ASSERT_EQ(first_duplicate->second, uint64_t(131));
    auto last_duplicate = sdv.select_iter(4);
    ASSERT_EQ(last_duplicate->first, uint64_t(3));
    ASSERT_EQ(last_duplicate->second, uint64_t(131));
    ASSERT_EQ(sdv.predecessor(131), last_duplicate);
    ASSERT_EQ(sdv.successor(131), first_duplicate);
}

TYPED_TEST(sd_vector_test, overfull_multiset)
{
    // A multiset with more values than universe size.
    std::vector<uint64_t> pos = { 0, 1, 1, 2, 2, 4, 5 };
    sd_vector_builder builder(pos.back() + 1, pos.size(), true);
    for (auto i : pos) {
        builder.set(i);
    }
    TypeParam sdv(builder);
    TypeParam second(pos.begin(), pos.end());

    // Basic tests.
    ASSERT_EQ(sdv.ones(), pos.size());
    ASSERT_EQ(second.ones(), pos.size());
    ASSERT_EQ(sdv.low.width(), 1);

    // Iterate forward.
    size_t expected = 0;
    for (auto iter = sdv.one_begin(), second_iter = second.one_begin(); iter != sdv.one_end(); ++iter, ++second_iter) {
        ASSERT_EQ(iter->first, expected);
        ASSERT_EQ(iter->second, pos[expected]);
        ASSERT_EQ(*iter, *second_iter);
        expected++;
    }
    ASSERT_EQ(expected, pos.size());
}

TYPED_TEST(sd_vector_test, equality)
{
    TypeParam original;
    {
        std::vector<uint64_t> pos = generate_positions(BV_SIZE, 9);
        sd_vector_builder builder(BV_SIZE, pos.size());
        for (auto i : pos) {
            builder.set(i);
        }
        original = TypeParam(builder);
    }

    TypeParam copy(original);
    ASSERT_EQ(copy, original);

    TypeParam sparser;
    {
        std::vector<uint64_t> pos = generate_positions(BV_SIZE, 12);
        sd_vector_builder builder(BV_SIZE, pos.size());
        for (auto i : pos) {
            builder.set(i);
        }
        sparser = TypeParam(builder);
    }
    ASSERT_NE(sparser, original);

    TypeParam shorter;
    {
        std::vector<uint64_t> pos = generate_positions(BV_SIZE / 2, 9);
        sd_vector_builder builder(BV_SIZE / 2, pos.size());
        for (auto i : pos) {
            builder.set(i);
        }
        shorter = TypeParam(builder);
    }
    ASSERT_NE(shorter, original);
}

TYPED_TEST(sd_vector_test, empty_builder)
{
    sd_vector_builder builder(BV_SIZE, 0UL);
    TypeParam sdv(builder);

    ASSERT_EQ(sdv.size(), BV_SIZE);
    ASSERT_EQ(sdv.ones(), size_t(0));
    for (size_t i=0; i < BV_SIZE; ++i) {
        ASSERT_FALSE((bool)sdv[i]);
    }

    ASSERT_EQ(sdv.one_begin(), sdv.one_end());
    ASSERT_EQ(sdv.select_iter(0), sdv.one_end());
    ASSERT_EQ(sdv.select_iter(1), sdv.one_end());
    ASSERT_EQ(sdv.predecessor(sdv.size()), sdv.one_end());
    ASSERT_EQ(sdv.successor(0), sdv.one_end());
}

TYPED_TEST(sd_vector_test, empty_one_iterator)
{
    TypeParam sdv;
    ASSERT_EQ(sdv.one_begin(), sdv.one_end());
    ASSERT_EQ(sdv.select_iter(0), sdv.one_end());
    ASSERT_EQ(sdv.select_iter(1), sdv.one_end());
    ASSERT_EQ(sdv.predecessor(sdv.size()), sdv.one_end());
    ASSERT_EQ(sdv.successor(0), sdv.one_end());
}

TYPED_TEST(sd_vector_test, builder_exceptions)
{
    {
        // Too many ones.
        ASSERT_THROW(sd_vector_builder(1024, 1025), std::runtime_error);
    }

    {
        // Position is too small.
        sd_vector_builder builder(1024, 3);
        builder.set(128);
        ASSERT_THROW(builder.set(128), std::runtime_error);
    }

    {
        // Position is too small in a multiset.
        sd_vector_builder builder(1024, 3, true);
        builder.set(128);
        builder.set(128); // This will not throw.
        ASSERT_THROW(builder.set(127), std::runtime_error);
    }

    {
        // Position is too large.
        sd_vector_builder builder(1024, 3);
        ASSERT_THROW(builder.set(1024), std::runtime_error);
    }

    {
        // Not full.
        sd_vector_builder builder(1024, 3);
        builder.set(128);
        builder.set(256);
        ASSERT_THROW(TypeParam{builder}, std::runtime_error);
    }
}

} // end namespace

int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

