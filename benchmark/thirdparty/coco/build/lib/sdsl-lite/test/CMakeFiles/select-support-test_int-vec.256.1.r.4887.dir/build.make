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

# Utility rule file for select-support-test_int-vec.256.1.r.4887.

# Include any custom commands dependencies for this target.
include lib/sdsl-lite/test/CMakeFiles/select-support-test_int-vec.256.1.r.4887.dir/compiler_depend.make

# Include the progress variables for this target.
include lib/sdsl-lite/test/CMakeFiles/select-support-test_int-vec.256.1.r.4887.dir/progress.make

lib/sdsl-lite/test/CMakeFiles/select-support-test_int-vec.256.1.r.4887: ../lib/sdsl-lite/test/test_cases/int-vec.256.1.r.4887
lib/sdsl-lite/test/CMakeFiles/select-support-test_int-vec.256.1.r.4887: ../lib/sdsl-lite/test/tmp
lib/sdsl-lite/test/CMakeFiles/select-support-test_int-vec.256.1.r.4887: lib/sdsl-lite/test/select_support_test
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/root/Dev/CoCo-trie/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Execute select-support-test on int-vec.256.1.r.4887."
	/root/Dev/CoCo-trie/build/lib/sdsl-lite/test/select_support_test /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.256.1.r.4887 /root/Dev/CoCo-trie/lib/sdsl-lite/test/tmp/select_support_test_int-vec.256.1.r.4887 /root/Dev/CoCo-trie/lib/sdsl-lite/test/tmp

../lib/sdsl-lite/test/test_cases/int-vec.256.1.r.4887: lib/sdsl-lite/test/int_vector_generator
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/root/Dev/CoCo-trie/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Generating test case int-vec.256.1.r.4887."
	/root/Dev/CoCo-trie/build/lib/sdsl-lite/test/int_vector_generator /root/Dev/CoCo-trie/lib/sdsl-lite/test/test_cases/int-vec.256.1.r.4887 256 1 r 4887

select-support-test_int-vec.256.1.r.4887: lib/sdsl-lite/test/CMakeFiles/select-support-test_int-vec.256.1.r.4887
select-support-test_int-vec.256.1.r.4887: ../lib/sdsl-lite/test/test_cases/int-vec.256.1.r.4887
select-support-test_int-vec.256.1.r.4887: lib/sdsl-lite/test/CMakeFiles/select-support-test_int-vec.256.1.r.4887.dir/build.make
.PHONY : select-support-test_int-vec.256.1.r.4887

# Rule to build all files generated by this target.
lib/sdsl-lite/test/CMakeFiles/select-support-test_int-vec.256.1.r.4887.dir/build: select-support-test_int-vec.256.1.r.4887
.PHONY : lib/sdsl-lite/test/CMakeFiles/select-support-test_int-vec.256.1.r.4887.dir/build

lib/sdsl-lite/test/CMakeFiles/select-support-test_int-vec.256.1.r.4887.dir/clean:
	cd /root/Dev/CoCo-trie/build/lib/sdsl-lite/test && $(CMAKE_COMMAND) -P CMakeFiles/select-support-test_int-vec.256.1.r.4887.dir/cmake_clean.cmake
.PHONY : lib/sdsl-lite/test/CMakeFiles/select-support-test_int-vec.256.1.r.4887.dir/clean

lib/sdsl-lite/test/CMakeFiles/select-support-test_int-vec.256.1.r.4887.dir/depend:
	cd /root/Dev/CoCo-trie/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/Dev/CoCo-trie /root/Dev/CoCo-trie/lib/sdsl-lite/test /root/Dev/CoCo-trie/build /root/Dev/CoCo-trie/build/lib/sdsl-lite/test /root/Dev/CoCo-trie/build/lib/sdsl-lite/test/CMakeFiles/select-support-test_int-vec.256.1.r.4887.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : lib/sdsl-lite/test/CMakeFiles/select-support-test_int-vec.256.1.r.4887.dir/depend

