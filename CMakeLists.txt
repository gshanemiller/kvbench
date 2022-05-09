cmake_minimum_required(VERSION 3.16)
project(reinvent)

# Define location of build artifacts
#
set(CMAKE_COMPILE_PDB_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_PDB_OUTPUT_DIRECTORY         ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY     ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY     ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY     ${CMAKE_SOURCE_DIR}/build)

#
# Set C++ project wide compilation options
#
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-diagnostics-color")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

#
# Set C project wide compilation options
#
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -fno-diagnostics-color")
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -O3")

add_subdirectory(src)
