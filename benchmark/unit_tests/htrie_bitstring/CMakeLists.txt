enable_testing()

set(UNIT_TEST_TASK "test_benchmark_htrie_bitstring.tsk")

set(TEST_SOURCES
  ./test_benchmark_htrie_bitstring.cpp
  ../../src/trie/benchmark_htrie_bitstring.cpp
  ../../src/trie/benchmark_htrie_bititerator.cpp
)

add_executable(${UNIT_TEST_TASK} ${TEST_SOURCES})

target_compile_options(${UNIT_TEST_TASK} PUBLIC -g)
target_compile_options(${UNIT_TEST_TASK} PUBLIC -O0)

target_include_directories(${UNIT_TEST_TASK} PUBLIC ../../src)
target_include_directories(${UNIT_TEST_TASK} PUBLIC /usr/local/include)
target_include_directories(${UNIT_TEST_TASK} PUBLIC ../../thirdparty/mimalloc/include)

target_link_directories(${UNIT_TEST_TASK} PUBLIC /usr/local/lib)

target_link_libraries(${UNIT_TEST_TASK} gtest gtest_main)
