# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /mnt/c/Users/anna/desktop/tests/lab2

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mnt/c/Users/anna/desktop/tests/lab2/build

# Include any dependencies generated for this target.
include CMakeFiles/peer.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/peer.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/peer.dir/flags.make

CMakeFiles/peer.dir/main.c.o: CMakeFiles/peer.dir/flags.make
CMakeFiles/peer.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/c/Users/anna/desktop/tests/lab2/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/peer.dir/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/peer.dir/main.c.o   -c /mnt/c/Users/anna/desktop/tests/lab2/main.c

CMakeFiles/peer.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/peer.dir/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/c/Users/anna/desktop/tests/lab2/main.c > CMakeFiles/peer.dir/main.c.i

CMakeFiles/peer.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/peer.dir/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/c/Users/anna/desktop/tests/lab2/main.c -o CMakeFiles/peer.dir/main.c.s

CMakeFiles/peer.dir/main.c.o.requires:

.PHONY : CMakeFiles/peer.dir/main.c.o.requires

CMakeFiles/peer.dir/main.c.o.provides: CMakeFiles/peer.dir/main.c.o.requires
	$(MAKE) -f CMakeFiles/peer.dir/build.make CMakeFiles/peer.dir/main.c.o.provides.build
.PHONY : CMakeFiles/peer.dir/main.c.o.provides

CMakeFiles/peer.dir/main.c.o.provides.build: CMakeFiles/peer.dir/main.c.o


CMakeFiles/peer.dir/ipc.c.o: CMakeFiles/peer.dir/flags.make
CMakeFiles/peer.dir/ipc.c.o: ../ipc.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/c/Users/anna/desktop/tests/lab2/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/peer.dir/ipc.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/peer.dir/ipc.c.o   -c /mnt/c/Users/anna/desktop/tests/lab2/ipc.c

CMakeFiles/peer.dir/ipc.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/peer.dir/ipc.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/c/Users/anna/desktop/tests/lab2/ipc.c > CMakeFiles/peer.dir/ipc.c.i

CMakeFiles/peer.dir/ipc.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/peer.dir/ipc.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/c/Users/anna/desktop/tests/lab2/ipc.c -o CMakeFiles/peer.dir/ipc.c.s

CMakeFiles/peer.dir/ipc.c.o.requires:

.PHONY : CMakeFiles/peer.dir/ipc.c.o.requires

CMakeFiles/peer.dir/ipc.c.o.provides: CMakeFiles/peer.dir/ipc.c.o.requires
	$(MAKE) -f CMakeFiles/peer.dir/build.make CMakeFiles/peer.dir/ipc.c.o.provides.build
.PHONY : CMakeFiles/peer.dir/ipc.c.o.provides

CMakeFiles/peer.dir/ipc.c.o.provides.build: CMakeFiles/peer.dir/ipc.c.o


# Object files for target peer
peer_OBJECTS = \
"CMakeFiles/peer.dir/main.c.o" \
"CMakeFiles/peer.dir/ipc.c.o"

# External object files for target peer
peer_EXTERNAL_OBJECTS =

peer: CMakeFiles/peer.dir/main.c.o
peer: CMakeFiles/peer.dir/ipc.c.o
peer: CMakeFiles/peer.dir/build.make
peer: CMakeFiles/peer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/mnt/c/Users/anna/desktop/tests/lab2/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable peer"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/peer.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/peer.dir/build: peer

.PHONY : CMakeFiles/peer.dir/build

CMakeFiles/peer.dir/requires: CMakeFiles/peer.dir/main.c.o.requires
CMakeFiles/peer.dir/requires: CMakeFiles/peer.dir/ipc.c.o.requires

.PHONY : CMakeFiles/peer.dir/requires

CMakeFiles/peer.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/peer.dir/cmake_clean.cmake
.PHONY : CMakeFiles/peer.dir/clean

CMakeFiles/peer.dir/depend:
	cd /mnt/c/Users/anna/desktop/tests/lab2/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/c/Users/anna/desktop/tests/lab2 /mnt/c/Users/anna/desktop/tests/lab2 /mnt/c/Users/anna/desktop/tests/lab2/build /mnt/c/Users/anna/desktop/tests/lab2/build /mnt/c/Users/anna/desktop/tests/lab2/build/CMakeFiles/peer.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/peer.dir/depend

