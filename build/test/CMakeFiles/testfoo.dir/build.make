# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.25

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
CMAKE_COMMAND = /usr/bin/cmake.exe

# The command to remove a file.
RM = /usr/bin/cmake.exe -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /cygdrive/c/Users/Admin/pdev/rpn/HIP35

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /cygdrive/c/Users/Admin/pdev/rpn/HIP35/build

# Include any dependencies generated for this target.
include test/CMakeFiles/testfoo.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include test/CMakeFiles/testfoo.dir/compiler_depend.make

# Include the progress variables for this target.
include test/CMakeFiles/testfoo.dir/progress.make

# Include the compile flags for this target's objects.
include test/CMakeFiles/testfoo.dir/flags.make

test/CMakeFiles/testfoo.dir/test.cpp.o: test/CMakeFiles/testfoo.dir/flags.make
test/CMakeFiles/testfoo.dir/test.cpp.o: /cygdrive/c/Users/Admin/pdev/rpn/HIP35/test/test.cpp
test/CMakeFiles/testfoo.dir/test.cpp.o: test/CMakeFiles/testfoo.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/cygdrive/c/Users/Admin/pdev/rpn/HIP35/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object test/CMakeFiles/testfoo.dir/test.cpp.o"
	cd /cygdrive/c/Users/Admin/pdev/rpn/HIP35/build/test && /usr/bin/c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT test/CMakeFiles/testfoo.dir/test.cpp.o -MF CMakeFiles/testfoo.dir/test.cpp.o.d -o CMakeFiles/testfoo.dir/test.cpp.o -c /cygdrive/c/Users/Admin/pdev/rpn/HIP35/test/test.cpp

test/CMakeFiles/testfoo.dir/test.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/testfoo.dir/test.cpp.i"
	cd /cygdrive/c/Users/Admin/pdev/rpn/HIP35/build/test && /usr/bin/c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /cygdrive/c/Users/Admin/pdev/rpn/HIP35/test/test.cpp > CMakeFiles/testfoo.dir/test.cpp.i

test/CMakeFiles/testfoo.dir/test.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/testfoo.dir/test.cpp.s"
	cd /cygdrive/c/Users/Admin/pdev/rpn/HIP35/build/test && /usr/bin/c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /cygdrive/c/Users/Admin/pdev/rpn/HIP35/test/test.cpp -o CMakeFiles/testfoo.dir/test.cpp.s

# Object files for target testfoo
testfoo_OBJECTS = \
"CMakeFiles/testfoo.dir/test.cpp.o"

# External object files for target testfoo
testfoo_EXTERNAL_OBJECTS =

test/testfoo.exe: test/CMakeFiles/testfoo.dir/test.cpp.o
test/testfoo.exe: test/CMakeFiles/testfoo.dir/build.make
test/testfoo.exe: lib/liblib.a
test/testfoo.exe: test/CMakeFiles/testfoo.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/cygdrive/c/Users/Admin/pdev/rpn/HIP35/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable testfoo.exe"
	cd /cygdrive/c/Users/Admin/pdev/rpn/HIP35/build/test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/testfoo.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/CMakeFiles/testfoo.dir/build: test/testfoo.exe
.PHONY : test/CMakeFiles/testfoo.dir/build

test/CMakeFiles/testfoo.dir/clean:
	cd /cygdrive/c/Users/Admin/pdev/rpn/HIP35/build/test && $(CMAKE_COMMAND) -P CMakeFiles/testfoo.dir/cmake_clean.cmake
.PHONY : test/CMakeFiles/testfoo.dir/clean

test/CMakeFiles/testfoo.dir/depend:
	cd /cygdrive/c/Users/Admin/pdev/rpn/HIP35/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /cygdrive/c/Users/Admin/pdev/rpn/HIP35 /cygdrive/c/Users/Admin/pdev/rpn/HIP35/test /cygdrive/c/Users/Admin/pdev/rpn/HIP35/build /cygdrive/c/Users/Admin/pdev/rpn/HIP35/build/test /cygdrive/c/Users/Admin/pdev/rpn/HIP35/build/test/CMakeFiles/testfoo.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : test/CMakeFiles/testfoo.dir/depend

