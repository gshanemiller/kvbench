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

# Include any dependencies generated for this target.
include lib/sdsl-lite/test/CMakeFiles/nn_dict_dynamic_test.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include lib/sdsl-lite/test/CMakeFiles/nn_dict_dynamic_test.dir/compiler_depend.make

# Include the progress variables for this target.
include lib/sdsl-lite/test/CMakeFiles/nn_dict_dynamic_test.dir/progress.make

# Include the compile flags for this target's objects.
include lib/sdsl-lite/test/CMakeFiles/nn_dict_dynamic_test.dir/flags.make

lib/sdsl-lite/test/CMakeFiles/nn_dict_dynamic_test.dir/nn_dict_dynamic_test.cpp.o: lib/sdsl-lite/test/CMakeFiles/nn_dict_dynamic_test.dir/flags.make
lib/sdsl-lite/test/CMakeFiles/nn_dict_dynamic_test.dir/nn_dict_dynamic_test.cpp.o: ../lib/sdsl-lite/test/nn_dict_dynamic_test.cpp
lib/sdsl-lite/test/CMakeFiles/nn_dict_dynamic_test.dir/nn_dict_dynamic_test.cpp.o: lib/sdsl-lite/test/CMakeFiles/nn_dict_dynamic_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/Dev/CoCo-trie/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object lib/sdsl-lite/test/CMakeFiles/nn_dict_dynamic_test.dir/nn_dict_dynamic_test.cpp.o"
	cd /root/Dev/CoCo-trie/build/lib/sdsl-lite/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT lib/sdsl-lite/test/CMakeFiles/nn_dict_dynamic_test.dir/nn_dict_dynamic_test.cpp.o -MF CMakeFiles/nn_dict_dynamic_test.dir/nn_dict_dynamic_test.cpp.o.d -o CMakeFiles/nn_dict_dynamic_test.dir/nn_dict_dynamic_test.cpp.o -c /root/Dev/CoCo-trie/lib/sdsl-lite/test/nn_dict_dynamic_test.cpp

lib/sdsl-lite/test/CMakeFiles/nn_dict_dynamic_test.dir/nn_dict_dynamic_test.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/nn_dict_dynamic_test.dir/nn_dict_dynamic_test.cpp.i"
	cd /root/Dev/CoCo-trie/build/lib/sdsl-lite/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/Dev/CoCo-trie/lib/sdsl-lite/test/nn_dict_dynamic_test.cpp > CMakeFiles/nn_dict_dynamic_test.dir/nn_dict_dynamic_test.cpp.i

lib/sdsl-lite/test/CMakeFiles/nn_dict_dynamic_test.dir/nn_dict_dynamic_test.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/nn_dict_dynamic_test.dir/nn_dict_dynamic_test.cpp.s"
	cd /root/Dev/CoCo-trie/build/lib/sdsl-lite/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/Dev/CoCo-trie/lib/sdsl-lite/test/nn_dict_dynamic_test.cpp -o CMakeFiles/nn_dict_dynamic_test.dir/nn_dict_dynamic_test.cpp.s

# Object files for target nn_dict_dynamic_test
nn_dict_dynamic_test_OBJECTS = \
"CMakeFiles/nn_dict_dynamic_test.dir/nn_dict_dynamic_test.cpp.o"

# External object files for target nn_dict_dynamic_test
nn_dict_dynamic_test_EXTERNAL_OBJECTS =

lib/sdsl-lite/test/nn_dict_dynamic_test: lib/sdsl-lite/test/CMakeFiles/nn_dict_dynamic_test.dir/nn_dict_dynamic_test.cpp.o
lib/sdsl-lite/test/nn_dict_dynamic_test: lib/sdsl-lite/test/CMakeFiles/nn_dict_dynamic_test.dir/build.make
lib/sdsl-lite/test/nn_dict_dynamic_test: lib/sdsl-lite/lib/libsdsl.a
lib/sdsl-lite/test/nn_dict_dynamic_test: lib/sdsl-lite/external/googletest/googletest/libgtest.a
lib/sdsl-lite/test/nn_dict_dynamic_test: lib/sdsl-lite/external/libdivsufsort/lib/libdivsufsort.a
lib/sdsl-lite/test/nn_dict_dynamic_test: lib/sdsl-lite/external/libdivsufsort/lib/libdivsufsort64.a
lib/sdsl-lite/test/nn_dict_dynamic_test: lib/sdsl-lite/test/CMakeFiles/nn_dict_dynamic_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/root/Dev/CoCo-trie/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable nn_dict_dynamic_test"
	cd /root/Dev/CoCo-trie/build/lib/sdsl-lite/test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/nn_dict_dynamic_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
lib/sdsl-lite/test/CMakeFiles/nn_dict_dynamic_test.dir/build: lib/sdsl-lite/test/nn_dict_dynamic_test
.PHONY : lib/sdsl-lite/test/CMakeFiles/nn_dict_dynamic_test.dir/build

lib/sdsl-lite/test/CMakeFiles/nn_dict_dynamic_test.dir/clean:
	cd /root/Dev/CoCo-trie/build/lib/sdsl-lite/test && $(CMAKE_COMMAND) -P CMakeFiles/nn_dict_dynamic_test.dir/cmake_clean.cmake
.PHONY : lib/sdsl-lite/test/CMakeFiles/nn_dict_dynamic_test.dir/clean

lib/sdsl-lite/test/CMakeFiles/nn_dict_dynamic_test.dir/depend:
	cd /root/Dev/CoCo-trie/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/Dev/CoCo-trie /root/Dev/CoCo-trie/lib/sdsl-lite/test /root/Dev/CoCo-trie/build /root/Dev/CoCo-trie/build/lib/sdsl-lite/test /root/Dev/CoCo-trie/build/lib/sdsl-lite/test/CMakeFiles/nn_dict_dynamic_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : lib/sdsl-lite/test/CMakeFiles/nn_dict_dynamic_test.dir/depend

