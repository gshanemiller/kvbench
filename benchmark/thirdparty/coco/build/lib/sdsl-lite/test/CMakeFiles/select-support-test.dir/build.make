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

# Utility rule file for select-support-test.

# Include any custom commands dependencies for this target.
include lib/sdsl-lite/test/CMakeFiles/select-support-test.dir/compiler_depend.make

# Include the progress variables for this target.
include lib/sdsl-lite/test/CMakeFiles/select-support-test.dir/progress.make

lib/sdsl-lite/test/CMakeFiles/select-support-test:

select-support-test: lib/sdsl-lite/test/CMakeFiles/select-support-test
select-support-test: lib/sdsl-lite/test/CMakeFiles/select-support-test.dir/build.make
.PHONY : select-support-test

# Rule to build all files generated by this target.
lib/sdsl-lite/test/CMakeFiles/select-support-test.dir/build: select-support-test
.PHONY : lib/sdsl-lite/test/CMakeFiles/select-support-test.dir/build

lib/sdsl-lite/test/CMakeFiles/select-support-test.dir/clean:
	cd /root/Dev/CoCo-trie/build/lib/sdsl-lite/test && $(CMAKE_COMMAND) -P CMakeFiles/select-support-test.dir/cmake_clean.cmake
.PHONY : lib/sdsl-lite/test/CMakeFiles/select-support-test.dir/clean

lib/sdsl-lite/test/CMakeFiles/select-support-test.dir/depend:
	cd /root/Dev/CoCo-trie/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/Dev/CoCo-trie /root/Dev/CoCo-trie/lib/sdsl-lite/test /root/Dev/CoCo-trie/build /root/Dev/CoCo-trie/build/lib/sdsl-lite/test /root/Dev/CoCo-trie/build/lib/sdsl-lite/test/CMakeFiles/select-support-test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : lib/sdsl-lite/test/CMakeFiles/select-support-test.dir/depend

