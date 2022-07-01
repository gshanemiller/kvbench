enable_testing()

set(UNIT_TEST_TASK "test_benchmark_patricia_tree.tsk")

set(TEST_SOURCES
  ./test.cpp
  ../../src/benchmark_slice.cpp
  ../../src/thirdparty/patricia/patricia_tre.cpp
)

add_executable(${UNIT_TEST_TASK} ${TEST_SOURCES})

target_compile_options(${UNIT_TEST_TASK} PUBLIC -g)
target_compile_options(${UNIT_TEST_TASK} PUBLIC -O0)

target_include_directories(${UNIT_TEST_TASK} PUBLIC ../../src)
target_include_directories(${UNIT_TEST_TASK} PUBLIC /usr/local/include)

target_link_directories(${UNIT_TEST_TASK} PUBLIC /usr/local/lib)

target_link_libraries(${UNIT_TEST_TASK} gtest gtest_main mimalloc)
