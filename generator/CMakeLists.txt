cmake_minimum_required(VERSION 3.16)
project(KVGenerator)

set(GENERATOR_SOURCES
  ./src/main.cpp
)

set(GENERATOR_TARGET generator.tsk)
add_executable(${GENERATOR_TARGET} ${GENERATOR_SOURCES})
target_compile_options(${GENERATOR_TARGET} PUBLIC -g)
