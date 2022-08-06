enable_testing()

set(UNIT_TEST_TASK "test_benchmark_cradix_tree.tsk")

set(TEST_SOURCES
  ./test.cpp
  ../../src/benchmark_slice.cpp
  ../../thirdparty/cradix/src/cradix_constants.cpp
  ../../thirdparty/cradix/src/cradix_iterator.cpp
  ../../thirdparty/cradix/src/cradix_iterstate.cpp
  ../../thirdparty/cradix/src/cradix_memmanager.cpp
  ../../thirdparty/cradix/src/cradix_memstats.cpp
  ../../thirdparty/cradix/src/cradix_node256.cpp
  ../../thirdparty/cradix/src/cradix_tree.cpp
  ../../thirdparty/cradix/src/cradix_treestats.cpp
)

add_executable(${UNIT_TEST_TASK} ${TEST_SOURCES})

target_compile_options(${UNIT_TEST_TASK} PUBLIC -g)
target_compile_options(${UNIT_TEST_TASK} PUBLIC -O0)

target_compile_definitions(${UNIT_TEST_TASK} PUBLIC CRADIX_MEMMANAGER_RUNTIME_STATISTICS )

target_include_directories(${UNIT_TEST_TASK} PUBLIC ../../src)
target_include_directories(${UNIT_TEST_TASK} PUBLIC /usr/local/include)
target_include_directories(${UNIT_TEST_TASK} PUBLIC ../../thirdparty/cradix/src)

target_link_directories(${UNIT_TEST_TASK} PUBLIC /usr/local/lib)

target_link_libraries(${UNIT_TEST_TASK})
