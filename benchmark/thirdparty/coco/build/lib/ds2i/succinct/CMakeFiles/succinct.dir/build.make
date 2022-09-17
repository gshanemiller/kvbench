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
include lib/ds2i/succinct/CMakeFiles/succinct.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include lib/ds2i/succinct/CMakeFiles/succinct.dir/compiler_depend.make

# Include the progress variables for this target.
include lib/ds2i/succinct/CMakeFiles/succinct.dir/progress.make

# Include the compile flags for this target's objects.
include lib/ds2i/succinct/CMakeFiles/succinct.dir/flags.make

lib/ds2i/succinct/CMakeFiles/succinct.dir/rs_bit_vector.cpp.o: lib/ds2i/succinct/CMakeFiles/succinct.dir/flags.make
lib/ds2i/succinct/CMakeFiles/succinct.dir/rs_bit_vector.cpp.o: ../lib/ds2i/succinct/rs_bit_vector.cpp
lib/ds2i/succinct/CMakeFiles/succinct.dir/rs_bit_vector.cpp.o: lib/ds2i/succinct/CMakeFiles/succinct.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/Dev/CoCo-trie/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object lib/ds2i/succinct/CMakeFiles/succinct.dir/rs_bit_vector.cpp.o"
	cd /root/Dev/CoCo-trie/build/lib/ds2i/succinct && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT lib/ds2i/succinct/CMakeFiles/succinct.dir/rs_bit_vector.cpp.o -MF CMakeFiles/succinct.dir/rs_bit_vector.cpp.o.d -o CMakeFiles/succinct.dir/rs_bit_vector.cpp.o -c /root/Dev/CoCo-trie/lib/ds2i/succinct/rs_bit_vector.cpp

lib/ds2i/succinct/CMakeFiles/succinct.dir/rs_bit_vector.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/succinct.dir/rs_bit_vector.cpp.i"
	cd /root/Dev/CoCo-trie/build/lib/ds2i/succinct && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/Dev/CoCo-trie/lib/ds2i/succinct/rs_bit_vector.cpp > CMakeFiles/succinct.dir/rs_bit_vector.cpp.i

lib/ds2i/succinct/CMakeFiles/succinct.dir/rs_bit_vector.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/succinct.dir/rs_bit_vector.cpp.s"
	cd /root/Dev/CoCo-trie/build/lib/ds2i/succinct && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/Dev/CoCo-trie/lib/ds2i/succinct/rs_bit_vector.cpp -o CMakeFiles/succinct.dir/rs_bit_vector.cpp.s

lib/ds2i/succinct/CMakeFiles/succinct.dir/bp_vector.cpp.o: lib/ds2i/succinct/CMakeFiles/succinct.dir/flags.make
lib/ds2i/succinct/CMakeFiles/succinct.dir/bp_vector.cpp.o: ../lib/ds2i/succinct/bp_vector.cpp
lib/ds2i/succinct/CMakeFiles/succinct.dir/bp_vector.cpp.o: lib/ds2i/succinct/CMakeFiles/succinct.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/Dev/CoCo-trie/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object lib/ds2i/succinct/CMakeFiles/succinct.dir/bp_vector.cpp.o"
	cd /root/Dev/CoCo-trie/build/lib/ds2i/succinct && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT lib/ds2i/succinct/CMakeFiles/succinct.dir/bp_vector.cpp.o -MF CMakeFiles/succinct.dir/bp_vector.cpp.o.d -o CMakeFiles/succinct.dir/bp_vector.cpp.o -c /root/Dev/CoCo-trie/lib/ds2i/succinct/bp_vector.cpp

lib/ds2i/succinct/CMakeFiles/succinct.dir/bp_vector.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/succinct.dir/bp_vector.cpp.i"
	cd /root/Dev/CoCo-trie/build/lib/ds2i/succinct && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/Dev/CoCo-trie/lib/ds2i/succinct/bp_vector.cpp > CMakeFiles/succinct.dir/bp_vector.cpp.i

lib/ds2i/succinct/CMakeFiles/succinct.dir/bp_vector.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/succinct.dir/bp_vector.cpp.s"
	cd /root/Dev/CoCo-trie/build/lib/ds2i/succinct && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/Dev/CoCo-trie/lib/ds2i/succinct/bp_vector.cpp -o CMakeFiles/succinct.dir/bp_vector.cpp.s

# Object files for target succinct
succinct_OBJECTS = \
"CMakeFiles/succinct.dir/rs_bit_vector.cpp.o" \
"CMakeFiles/succinct.dir/bp_vector.cpp.o"

# External object files for target succinct
succinct_EXTERNAL_OBJECTS =

lib/ds2i/succinct/libsuccinct.a: lib/ds2i/succinct/CMakeFiles/succinct.dir/rs_bit_vector.cpp.o
lib/ds2i/succinct/libsuccinct.a: lib/ds2i/succinct/CMakeFiles/succinct.dir/bp_vector.cpp.o
lib/ds2i/succinct/libsuccinct.a: lib/ds2i/succinct/CMakeFiles/succinct.dir/build.make
lib/ds2i/succinct/libsuccinct.a: lib/ds2i/succinct/CMakeFiles/succinct.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/root/Dev/CoCo-trie/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX static library libsuccinct.a"
	cd /root/Dev/CoCo-trie/build/lib/ds2i/succinct && $(CMAKE_COMMAND) -P CMakeFiles/succinct.dir/cmake_clean_target.cmake
	cd /root/Dev/CoCo-trie/build/lib/ds2i/succinct && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/succinct.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
lib/ds2i/succinct/CMakeFiles/succinct.dir/build: lib/ds2i/succinct/libsuccinct.a
.PHONY : lib/ds2i/succinct/CMakeFiles/succinct.dir/build

lib/ds2i/succinct/CMakeFiles/succinct.dir/clean:
	cd /root/Dev/CoCo-trie/build/lib/ds2i/succinct && $(CMAKE_COMMAND) -P CMakeFiles/succinct.dir/cmake_clean.cmake
.PHONY : lib/ds2i/succinct/CMakeFiles/succinct.dir/clean

lib/ds2i/succinct/CMakeFiles/succinct.dir/depend:
	cd /root/Dev/CoCo-trie/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/Dev/CoCo-trie /root/Dev/CoCo-trie/lib/ds2i/succinct /root/Dev/CoCo-trie/build /root/Dev/CoCo-trie/build/lib/ds2i/succinct /root/Dev/CoCo-trie/build/lib/ds2i/succinct/CMakeFiles/succinct.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : lib/ds2i/succinct/CMakeFiles/succinct.dir/depend

