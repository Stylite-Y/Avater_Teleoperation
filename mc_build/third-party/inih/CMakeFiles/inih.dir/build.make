# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

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
CMAKE_SOURCE_DIR = /home/stylite-y/Documents/Master/Avater_Teleoperation

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/stylite-y/Documents/Master/Avater_Teleoperation/mc_build

# Include any dependencies generated for this target.
include third-party/inih/CMakeFiles/inih.dir/depend.make

# Include the progress variables for this target.
include third-party/inih/CMakeFiles/inih.dir/progress.make

# Include the compile flags for this target's objects.
include third-party/inih/CMakeFiles/inih.dir/flags.make

third-party/inih/CMakeFiles/inih.dir/ini.c.o: third-party/inih/CMakeFiles/inih.dir/flags.make
third-party/inih/CMakeFiles/inih.dir/ini.c.o: ../third-party/inih/ini.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/stylite-y/Documents/Master/Avater_Teleoperation/mc_build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object third-party/inih/CMakeFiles/inih.dir/ini.c.o"
	cd /home/stylite-y/Documents/Master/Avater_Teleoperation/mc_build/third-party/inih && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/inih.dir/ini.c.o   -c /home/stylite-y/Documents/Master/Avater_Teleoperation/third-party/inih/ini.c

third-party/inih/CMakeFiles/inih.dir/ini.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/inih.dir/ini.c.i"
	cd /home/stylite-y/Documents/Master/Avater_Teleoperation/mc_build/third-party/inih && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/stylite-y/Documents/Master/Avater_Teleoperation/third-party/inih/ini.c > CMakeFiles/inih.dir/ini.c.i

third-party/inih/CMakeFiles/inih.dir/ini.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/inih.dir/ini.c.s"
	cd /home/stylite-y/Documents/Master/Avater_Teleoperation/mc_build/third-party/inih && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/stylite-y/Documents/Master/Avater_Teleoperation/third-party/inih/ini.c -o CMakeFiles/inih.dir/ini.c.s

third-party/inih/CMakeFiles/inih.dir/ini.c.o.requires:

.PHONY : third-party/inih/CMakeFiles/inih.dir/ini.c.o.requires

third-party/inih/CMakeFiles/inih.dir/ini.c.o.provides: third-party/inih/CMakeFiles/inih.dir/ini.c.o.requires
	$(MAKE) -f third-party/inih/CMakeFiles/inih.dir/build.make third-party/inih/CMakeFiles/inih.dir/ini.c.o.provides.build
.PHONY : third-party/inih/CMakeFiles/inih.dir/ini.c.o.provides

third-party/inih/CMakeFiles/inih.dir/ini.c.o.provides.build: third-party/inih/CMakeFiles/inih.dir/ini.c.o


third-party/inih/CMakeFiles/inih.dir/INIReader.cpp.o: third-party/inih/CMakeFiles/inih.dir/flags.make
third-party/inih/CMakeFiles/inih.dir/INIReader.cpp.o: ../third-party/inih/INIReader.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/stylite-y/Documents/Master/Avater_Teleoperation/mc_build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object third-party/inih/CMakeFiles/inih.dir/INIReader.cpp.o"
	cd /home/stylite-y/Documents/Master/Avater_Teleoperation/mc_build/third-party/inih && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/inih.dir/INIReader.cpp.o -c /home/stylite-y/Documents/Master/Avater_Teleoperation/third-party/inih/INIReader.cpp

third-party/inih/CMakeFiles/inih.dir/INIReader.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/inih.dir/INIReader.cpp.i"
	cd /home/stylite-y/Documents/Master/Avater_Teleoperation/mc_build/third-party/inih && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/stylite-y/Documents/Master/Avater_Teleoperation/third-party/inih/INIReader.cpp > CMakeFiles/inih.dir/INIReader.cpp.i

third-party/inih/CMakeFiles/inih.dir/INIReader.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/inih.dir/INIReader.cpp.s"
	cd /home/stylite-y/Documents/Master/Avater_Teleoperation/mc_build/third-party/inih && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/stylite-y/Documents/Master/Avater_Teleoperation/third-party/inih/INIReader.cpp -o CMakeFiles/inih.dir/INIReader.cpp.s

third-party/inih/CMakeFiles/inih.dir/INIReader.cpp.o.requires:

.PHONY : third-party/inih/CMakeFiles/inih.dir/INIReader.cpp.o.requires

third-party/inih/CMakeFiles/inih.dir/INIReader.cpp.o.provides: third-party/inih/CMakeFiles/inih.dir/INIReader.cpp.o.requires
	$(MAKE) -f third-party/inih/CMakeFiles/inih.dir/build.make third-party/inih/CMakeFiles/inih.dir/INIReader.cpp.o.provides.build
.PHONY : third-party/inih/CMakeFiles/inih.dir/INIReader.cpp.o.provides

third-party/inih/CMakeFiles/inih.dir/INIReader.cpp.o.provides.build: third-party/inih/CMakeFiles/inih.dir/INIReader.cpp.o


# Object files for target inih
inih_OBJECTS = \
"CMakeFiles/inih.dir/ini.c.o" \
"CMakeFiles/inih.dir/INIReader.cpp.o"

# External object files for target inih
inih_EXTERNAL_OBJECTS =

third-party/inih/libinih.so: third-party/inih/CMakeFiles/inih.dir/ini.c.o
third-party/inih/libinih.so: third-party/inih/CMakeFiles/inih.dir/INIReader.cpp.o
third-party/inih/libinih.so: third-party/inih/CMakeFiles/inih.dir/build.make
third-party/inih/libinih.so: third-party/inih/CMakeFiles/inih.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/stylite-y/Documents/Master/Avater_Teleoperation/mc_build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX shared library libinih.so"
	cd /home/stylite-y/Documents/Master/Avater_Teleoperation/mc_build/third-party/inih && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/inih.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
third-party/inih/CMakeFiles/inih.dir/build: third-party/inih/libinih.so

.PHONY : third-party/inih/CMakeFiles/inih.dir/build

third-party/inih/CMakeFiles/inih.dir/requires: third-party/inih/CMakeFiles/inih.dir/ini.c.o.requires
third-party/inih/CMakeFiles/inih.dir/requires: third-party/inih/CMakeFiles/inih.dir/INIReader.cpp.o.requires

.PHONY : third-party/inih/CMakeFiles/inih.dir/requires

third-party/inih/CMakeFiles/inih.dir/clean:
	cd /home/stylite-y/Documents/Master/Avater_Teleoperation/mc_build/third-party/inih && $(CMAKE_COMMAND) -P CMakeFiles/inih.dir/cmake_clean.cmake
.PHONY : third-party/inih/CMakeFiles/inih.dir/clean

third-party/inih/CMakeFiles/inih.dir/depend:
	cd /home/stylite-y/Documents/Master/Avater_Teleoperation/mc_build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/stylite-y/Documents/Master/Avater_Teleoperation /home/stylite-y/Documents/Master/Avater_Teleoperation/third-party/inih /home/stylite-y/Documents/Master/Avater_Teleoperation/mc_build /home/stylite-y/Documents/Master/Avater_Teleoperation/mc_build/third-party/inih /home/stylite-y/Documents/Master/Avater_Teleoperation/mc_build/third-party/inih/CMakeFiles/inih.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : third-party/inih/CMakeFiles/inih.dir/depend

