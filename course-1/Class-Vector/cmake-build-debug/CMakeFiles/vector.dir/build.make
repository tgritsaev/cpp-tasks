# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.17

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

# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = "/Users/timgritsaev/Library/Application Support/JetBrains/Toolbox/apps/CLion/ch-0/202.6948.80/CLion.app/Contents/bin/cmake/mac/bin/cmake"

# The command to remove a file.
RM = "/Users/timgritsaev/Library/Application Support/JetBrains/Toolbox/apps/CLion/ch-0/202.6948.80/CLion.app/Contents/bin/cmake/mac/bin/cmake" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/timgritsaev/CLionProjects/vector

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/timgritsaev/CLionProjects/vector/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/vector.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/vector.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/vector.dir/flags.make

CMakeFiles/vector.dir/main.cpp.o: CMakeFiles/vector.dir/flags.make
CMakeFiles/vector.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/timgritsaev/CLionProjects/vector/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/vector.dir/main.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/vector.dir/main.cpp.o -c /Users/timgritsaev/CLionProjects/vector/main.cpp

CMakeFiles/vector.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/vector.dir/main.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/timgritsaev/CLionProjects/vector/main.cpp > CMakeFiles/vector.dir/main.cpp.i

CMakeFiles/vector.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/vector.dir/main.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/timgritsaev/CLionProjects/vector/main.cpp -o CMakeFiles/vector.dir/main.cpp.s

# Object files for target vector
vector_OBJECTS = \
"CMakeFiles/vector.dir/main.cpp.o"

# External object files for target vector
vector_EXTERNAL_OBJECTS =

vector: CMakeFiles/vector.dir/main.cpp.o
vector: CMakeFiles/vector.dir/build.make
vector: CMakeFiles/vector.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/timgritsaev/CLionProjects/vector/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable vector"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/vector.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/vector.dir/build: vector

.PHONY : CMakeFiles/vector.dir/build

CMakeFiles/vector.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/vector.dir/cmake_clean.cmake
.PHONY : CMakeFiles/vector.dir/clean

CMakeFiles/vector.dir/depend:
	cd /Users/timgritsaev/CLionProjects/vector/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/timgritsaev/CLionProjects/vector /Users/timgritsaev/CLionProjects/vector /Users/timgritsaev/CLionProjects/vector/cmake-build-debug /Users/timgritsaev/CLionProjects/vector/cmake-build-debug /Users/timgritsaev/CLionProjects/vector/cmake-build-debug/CMakeFiles/vector.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/vector.dir/depend

