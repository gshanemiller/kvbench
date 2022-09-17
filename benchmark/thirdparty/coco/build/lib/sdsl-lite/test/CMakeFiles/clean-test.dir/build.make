# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /root/Dev/CoCo-trie

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /root/Dev/CoCo-trie/build

# Utility rule file for clean-test.

# Include any custom commands dependencies for this target.
include lib/sdsl-lite/test/CMakeFiles/clean-test.dir/compiler_depend.make

# Include the progress variables for this target.
include lib/sdsl-lite/test/CMakeFiles/clean-test.dir/progress.make

lib/sdsl-lite/test/CMakeFiles/clean-test:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/root/Dev/CoCo-trie/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Remove generated and downloaded test inputs."
	cd /root/Dev/CoCo-trie/build/lib/sdsl-lite/test && /usr/bin/cmake -E remove -f /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/faust.txt /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/zarathustra.txt /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/moby.int /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.0.1.0 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.1.1.0 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.1000000.1.0 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.1000000.1.1 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.7.1.1 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.8.1.0 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.9.1.1 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.10.1.0 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.11.1.1 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.12.1.0 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.13.1.1 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.14.1.0 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.15.1.1 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.8.1.r.17 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.16.1.r.42 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.32.1.r.111 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.64.1.r.222 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.128.1.r.73 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.256.1.r.4887 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.512.1.r.432 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.1024.1.r.898 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.2048.1.r.5432 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.4096.1.r.793 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.8192.1.r.1043 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.1000000.1.r.815 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/bit-vec.CRAFTED-32 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/bit-vec.CRAFTED-SPARSE-0 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/bit-vec.CRAFTED-SPARSE-1 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/bit-vec.CRAFTED-BLOCK-0 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/bit-vec.CRAFTED-BLOCK-1 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/bit-vec.CRAFTED-MAT-SELECT /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec-sa.100000.18.r /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.0.1.0 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.1.1.0 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.1000000.1.0 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.1000000.1.1 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.7.1.1 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.8.1.0 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.9.1.1 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.10.1.0 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.11.1.1 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.12.1.0 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.13.1.1 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.14.1.0 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.15.1.1 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.8.1.r.17 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.16.1.r.42 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.32.1.r.111 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.64.1.r.222 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.128.1.r.73 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.256.1.r.4887 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.512.1.r.432 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.1024.1.r.898 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.2048.1.r.5432 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.4096.1.r.793 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.8192.1.r.1043 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.1000000.1.r.815 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/bit-vec.CRAFTED-32 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/bit-vec.CRAFTED-SPARSE-0 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/bit-vec.CRAFTED-SPARSE-1 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/bit-vec.CRAFTED-BLOCK-0 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/bit-vec.CRAFTED-BLOCK-1 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/bit-vec.CRAFTED-MAT-SELECT /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.0.1.0 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.1.64.42 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.1000000.64.i /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.1000000.32.i.42 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.1000000.64.i.17 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.0.1.0 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.1.1.0 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.1000000.1.0 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.1000000.1.1 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.7.1.1 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.8.1.0 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.9.1.1 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.10.1.0 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.11.1.1 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.12.1.0 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.13.1.1 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.14.1.0 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.15.1.1 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.8.1.r.17 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.16.1.r.42 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.32.1.r.111 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.64.1.r.222 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.128.1.r.73 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.256.1.r.4887 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.512.1.r.432 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.1024.1.r.898 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.2048.1.r.5432 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.4096.1.r.793 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.8192.1.r.1043 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.1000000.1.r.815 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/bit-vec.CRAFTED-32 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/bit-vec.CRAFTED-SPARSE-0 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/bit-vec.CRAFTED-SPARSE-1 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/bit-vec.CRAFTED-BLOCK-0 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/bit-vec.CRAFTED-BLOCK-1 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/bit-vec.CRAFTED-MAT-SELECT /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.0.1.0 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.1023.1.0 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.100023.1.0 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.64.2.0 /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.100000.18.r

clean-test: lib/sdsl-lite/test/CMakeFiles/clean-test
clean-test: lib/sdsl-lite/test/CMakeFiles/clean-test.dir/build.make
.PHONY : clean-test

# Rule to build all files generated by this target.
lib/sdsl-lite/test/CMakeFiles/clean-test.dir/build: clean-test
.PHONY : lib/sdsl-lite/test/CMakeFiles/clean-test.dir/build

lib/sdsl-lite/test/CMakeFiles/clean-test.dir/clean:
	cd /root/Dev/CoCo-trie/build/lib/sdsl-lite/test && $(CMAKE_COMMAND) -P CMakeFiles/clean-test.dir/cmake_clean.cmake
.PHONY : lib/sdsl-lite/test/CMakeFiles/clean-test.dir/clean

lib/sdsl-lite/test/CMakeFiles/clean-test.dir/depend:
	cd /root/Dev/CoCo-trie/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/Dev/CoCo-trie /root/Dev/CoCo-trie/lib/sdsl-lite/test /root/Dev/CoCo-trie/build /root/Dev/CoCo-trie/build/lib/sdsl-lite/test /root/Dev/CoCo-trie/build/lib/sdsl-lite/test/CMakeFiles/clean-test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : lib/sdsl-lite/test/CMakeFiles/clean-test.dir/depend

