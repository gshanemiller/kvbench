cmake_minimum_required(VERSION 3.13)
project(CoCo-trie)

set(CMAKE_CXX_STANDARD 20)

configure_file(
        lib/ds2i/ds2i_config.hpp.in
        lib/ds2i/ds2i_config.hpp
        ESCAPE_QUOTES)

find_package(Boost REQUIRED)

include_directories(include lib/ds2i)
add_subdirectory(lib/ds2i/succinct)
add_subdirectory(lib/sdsl-lite)
