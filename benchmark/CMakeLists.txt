cmake_minimum_required(VERSION 3.16)
project(KVBenchmark)

add_subdirectory(unit_tests)
add_subdirectory(thirdparty/mimalloc)

set(BENCHMARK_SOURCES
  ./src/main.cpp

  ./src/benchmark_report.cpp
  ./src/benchmark_config.cpp
  ./src/benchmark_cstr.cpp
  ./src/benchmark_cuckoo.cpp
  ./src/benchmark_f14.cpp
  ./src/benchmark_loadfile.cpp
  ./src/benchmark_slice.cpp
  ./src/benchmark_textscan.cpp
  ./src/benchmark_hot.cpp
  ./src/benchmark_art.cpp
  ./src/benchmark_patricia.cpp
  ./src/benchmark_cradix.cpp
  ./src/benchmark_cedar.cpp
  ./src/benchmark_wormhole.cpp
  ./src/benchmark_hattrie.cpp

  ./src/intel_skylake_pmu.cpp
  ./src/intel_pmu_stats.cpp

  ./thirdparty/xxhash/xxhash.c

  ./thirdparty/t1ha/t1ha.c

  ./thirdparty/cityhash/city.cc

  ./thirdparty/art/art.c

  ./thirdparty/wormhole/src/kv.c
  ./thirdparty/wormhole/src/lib.c
  ./thirdparty/wormhole/src/wh.c

  ./thirdparty/folly/ScopeGuard.cpp
  ./thirdparty/folly/lang/ToAscii.cpp
  ./thirdparty/folly/lang/SafeAssert.cpp
  ./thirdparty/folly/container/f14.cpp                                                                                             
  ./thirdparty/folly/container/detail/F14Table.cpp

  ./thirdparty/patricia/src/patricia_tree.cpp
  
  ./thirdparty/radix/src/radix_memmanager.cpp
  ./thirdparty/radix/src/radix.cpp

  ./thirdparty/cradix/src/cradix_constants.cpp
  ./thirdparty/cradix/src/cradix_iterator.cpp
  ./thirdparty/cradix/src/cradix_iterstate.cpp
  ./thirdparty/cradix/src/cradix_memmanager.cpp
  ./thirdparty/cradix/src/cradix_memstats.cpp
  ./thirdparty/cradix/src/cradix_node256.cpp
  ./thirdparty/cradix/src/cradix_tree.cpp
  ./thirdparty/cradix/src/cradix_treestats.cpp
  ./thirdparty/cradix/src/cradix_nodestats.cpp
)

find_library(HUGELIB
  NAMES hugetlbfs
  PATHS /usr/lib/x86_64-linux-gnu
  REQUIRED
)

set(BENCHMARK_TARGET benchmark.tsk)
add_executable(${BENCHMARK_TARGET} ${BENCHMARK_SOURCES})
# target_compile_options(${BENCHMARK_TARGET} PUBLIC -g -O0)
# target_compile_definitions(${BENCHMARK_TARGET} PUBLIC -DCRADIX_MEMMANAGER_RUNTIME_STATISTICS)
# target_compile_definitions(${BENCHMARK_TARGET} PUBLIC -DCRADIX_NODE_RUNTIME_STATISTICS)
target_include_directories(${BENCHMARK_TARGET} PUBLIC ./src)
target_include_directories(${BENCHMARK_TARGET} PUBLIC ./thirdparty)
target_include_directories(${BENCHMARK_TARGET} PUBLIC ./thirdparty/art)
target_include_directories(${BENCHMARK_TARGET} PUBLIC ./thirdparty/cuckoo)
target_include_directories(${BENCHMARK_TARGET} PUBLIC ./thirdparty/xxhash)
target_include_directories(${BENCHMARK_TARGET} PUBLIC ./thirdparty/t1ha)
target_include_directories(${BENCHMARK_TARGET} PUBLIC ./thirdparty/cityhash)
target_include_directories(${BENCHMARK_TARGET} PUBLIC ./thirdparty/folly)
target_include_directories(${BENCHMARK_TARGET} PUBLIC ./thirdparty/folly/container)
target_include_directories(${BENCHMARK_TARGET} PUBLIC ./thirdparty/hot/src)
target_include_directories(${BENCHMARK_TARGET} PUBLIC ./thirdparty/radix/src)
target_include_directories(${BENCHMARK_TARGET} PUBLIC ./thirdparty/patricia/src)
target_include_directories(${BENCHMARK_TARGET} PUBLIC ./thirdparty/cradix/src)
target_include_directories(${BENCHMARK_TARGET} PUBLIC ./thirdparty/ringbuffer/include)
target_include_directories(${BENCHMARK_TARGET} PUBLIC ./thirdparty/cedar/src)
target_include_directories(${BENCHMARK_TARGET} PUBLIC ./thirdparty/wormhole/src)
target_include_directories(${BENCHMARK_TARGET} PUBLIC ./thirdparty/hattrie/src)
target_include_directories(${BENCHMARK_TARGET} PUBLIC ./thirdparty/hattrie/src/array-hash)
target_link_libraries(${BENCHMARK_TARGET} PUBLIC ${HUGELIB})
target_link_libraries(${BENCHMARK_TARGET} PUBLIC pthread)
target_link_libraries(${BENCHMARK_TARGET} PUBLIC mimalloc-static)
