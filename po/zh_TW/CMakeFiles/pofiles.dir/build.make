# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.6

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canoncical targets will work.
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
CMAKE_COMMAND = /mnt/sda2/usr/bin/cmake

# The command to remove a file.
RM = /mnt/sda2/usr/bin/cmake -E remove -f

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/modax/src/konversation/konversation

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/modax/src/konversation/konversation

# Utility rule file for pofiles.

po/zh_TW/CMakeFiles/pofiles: po/zh_TW/konversation.gmo

po/zh_TW/konversation.gmo: po/zh_TW/konversation.po
	$(CMAKE_COMMAND) -E cmake_progress_report /home/modax/src/konversation/konversation/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Generating konversation.gmo"
	cd /home/modax/src/konversation/konversation/po/zh_TW && /mnt/sda2/usr/bin/msgfmt -o /home/modax/src/konversation/konversation/po/zh_TW/konversation.gmo /home/modax/src/konversation/konversation/po/zh_TW/konversation.po

pofiles: po/zh_TW/CMakeFiles/pofiles
pofiles: po/zh_TW/konversation.gmo
pofiles: po/zh_TW/CMakeFiles/pofiles.dir/build.make
.PHONY : pofiles

# Rule to build all files generated by this target.
po/zh_TW/CMakeFiles/pofiles.dir/build: pofiles
.PHONY : po/zh_TW/CMakeFiles/pofiles.dir/build

po/zh_TW/CMakeFiles/pofiles.dir/clean:
	cd /home/modax/src/konversation/konversation/po/zh_TW && $(CMAKE_COMMAND) -P CMakeFiles/pofiles.dir/cmake_clean.cmake
.PHONY : po/zh_TW/CMakeFiles/pofiles.dir/clean

po/zh_TW/CMakeFiles/pofiles.dir/depend:
	cd /home/modax/src/konversation/konversation && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/modax/src/konversation/konversation /home/modax/src/konversation/konversation/po/zh_TW /home/modax/src/konversation/konversation /home/modax/src/konversation/konversation/po/zh_TW /home/modax/src/konversation/konversation/po/zh_TW/CMakeFiles/pofiles.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : po/zh_TW/CMakeFiles/pofiles.dir/depend

