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
CMAKE_SOURCE_DIR = /home/ros/Documents/Masters_Jetson/Camera/src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ros/Documents/Masters_Jetson/Camera/build

# Include any dependencies generated for this target.
include Camera/CMakeFiles/Camera_node.dir/depend.make

# Include the progress variables for this target.
include Camera/CMakeFiles/Camera_node.dir/progress.make

# Include the compile flags for this target's objects.
include Camera/CMakeFiles/Camera_node.dir/flags.make

Camera/CMakeFiles/Camera_node.dir/src/camera_node.cpp.o: Camera/CMakeFiles/Camera_node.dir/flags.make
Camera/CMakeFiles/Camera_node.dir/src/camera_node.cpp.o: /home/ros/Documents/Masters_Jetson/Camera/src/Camera/src/camera_node.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ros/Documents/Masters_Jetson/Camera/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object Camera/CMakeFiles/Camera_node.dir/src/camera_node.cpp.o"
	cd /home/ros/Documents/Masters_Jetson/Camera/build/Camera && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Camera_node.dir/src/camera_node.cpp.o -c /home/ros/Documents/Masters_Jetson/Camera/src/Camera/src/camera_node.cpp

Camera/CMakeFiles/Camera_node.dir/src/camera_node.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Camera_node.dir/src/camera_node.cpp.i"
	cd /home/ros/Documents/Masters_Jetson/Camera/build/Camera && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ros/Documents/Masters_Jetson/Camera/src/Camera/src/camera_node.cpp > CMakeFiles/Camera_node.dir/src/camera_node.cpp.i

Camera/CMakeFiles/Camera_node.dir/src/camera_node.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Camera_node.dir/src/camera_node.cpp.s"
	cd /home/ros/Documents/Masters_Jetson/Camera/build/Camera && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ros/Documents/Masters_Jetson/Camera/src/Camera/src/camera_node.cpp -o CMakeFiles/Camera_node.dir/src/camera_node.cpp.s

Camera/CMakeFiles/Camera_node.dir/src/camera_node.cpp.o.requires:

.PHONY : Camera/CMakeFiles/Camera_node.dir/src/camera_node.cpp.o.requires

Camera/CMakeFiles/Camera_node.dir/src/camera_node.cpp.o.provides: Camera/CMakeFiles/Camera_node.dir/src/camera_node.cpp.o.requires
	$(MAKE) -f Camera/CMakeFiles/Camera_node.dir/build.make Camera/CMakeFiles/Camera_node.dir/src/camera_node.cpp.o.provides.build
.PHONY : Camera/CMakeFiles/Camera_node.dir/src/camera_node.cpp.o.provides

Camera/CMakeFiles/Camera_node.dir/src/camera_node.cpp.o.provides.build: Camera/CMakeFiles/Camera_node.dir/src/camera_node.cpp.o


# Object files for target Camera_node
Camera_node_OBJECTS = \
"CMakeFiles/Camera_node.dir/src/camera_node.cpp.o"

# External object files for target Camera_node
Camera_node_EXTERNAL_OBJECTS =

/home/ros/Documents/Masters_Jetson/Camera/devel/lib/Camera/Camera_node: Camera/CMakeFiles/Camera_node.dir/src/camera_node.cpp.o
/home/ros/Documents/Masters_Jetson/Camera/devel/lib/Camera/Camera_node: Camera/CMakeFiles/Camera_node.dir/build.make
/home/ros/Documents/Masters_Jetson/Camera/devel/lib/Camera/Camera_node: /opt/ros/melodic/lib/libroscpp.so
/home/ros/Documents/Masters_Jetson/Camera/devel/lib/Camera/Camera_node: /usr/lib/aarch64-linux-gnu/libboost_filesystem.so
/home/ros/Documents/Masters_Jetson/Camera/devel/lib/Camera/Camera_node: /opt/ros/melodic/lib/libxmlrpcpp.so
/home/ros/Documents/Masters_Jetson/Camera/devel/lib/Camera/Camera_node: /opt/ros/melodic/lib/libcv_bridge.so
/home/ros/Documents/Masters_Jetson/Camera/devel/lib/Camera/Camera_node: /usr/lib/aarch64-linux-gnu/libopencv_core.so.3.2.0
/home/ros/Documents/Masters_Jetson/Camera/devel/lib/Camera/Camera_node: /usr/lib/aarch64-linux-gnu/libopencv_imgproc.so.3.2.0
/home/ros/Documents/Masters_Jetson/Camera/devel/lib/Camera/Camera_node: /usr/lib/aarch64-linux-gnu/libopencv_imgcodecs.so.3.2.0
/home/ros/Documents/Masters_Jetson/Camera/devel/lib/Camera/Camera_node: /opt/ros/melodic/lib/librosconsole.so
/home/ros/Documents/Masters_Jetson/Camera/devel/lib/Camera/Camera_node: /opt/ros/melodic/lib/librosconsole_log4cxx.so
/home/ros/Documents/Masters_Jetson/Camera/devel/lib/Camera/Camera_node: /opt/ros/melodic/lib/librosconsole_backend_interface.so
/home/ros/Documents/Masters_Jetson/Camera/devel/lib/Camera/Camera_node: /usr/lib/aarch64-linux-gnu/liblog4cxx.so
/home/ros/Documents/Masters_Jetson/Camera/devel/lib/Camera/Camera_node: /usr/lib/aarch64-linux-gnu/libboost_regex.so
/home/ros/Documents/Masters_Jetson/Camera/devel/lib/Camera/Camera_node: /opt/ros/melodic/lib/libroscpp_serialization.so
/home/ros/Documents/Masters_Jetson/Camera/devel/lib/Camera/Camera_node: /opt/ros/melodic/lib/librostime.so
/home/ros/Documents/Masters_Jetson/Camera/devel/lib/Camera/Camera_node: /opt/ros/melodic/lib/libcpp_common.so
/home/ros/Documents/Masters_Jetson/Camera/devel/lib/Camera/Camera_node: /usr/lib/aarch64-linux-gnu/libboost_system.so
/home/ros/Documents/Masters_Jetson/Camera/devel/lib/Camera/Camera_node: /usr/lib/aarch64-linux-gnu/libboost_thread.so
/home/ros/Documents/Masters_Jetson/Camera/devel/lib/Camera/Camera_node: /usr/lib/aarch64-linux-gnu/libboost_chrono.so
/home/ros/Documents/Masters_Jetson/Camera/devel/lib/Camera/Camera_node: /usr/lib/aarch64-linux-gnu/libboost_date_time.so
/home/ros/Documents/Masters_Jetson/Camera/devel/lib/Camera/Camera_node: /usr/lib/aarch64-linux-gnu/libboost_atomic.so
/home/ros/Documents/Masters_Jetson/Camera/devel/lib/Camera/Camera_node: /usr/lib/aarch64-linux-gnu/libpthread.so
/home/ros/Documents/Masters_Jetson/Camera/devel/lib/Camera/Camera_node: /usr/lib/aarch64-linux-gnu/libconsole_bridge.so.0.4
/home/ros/Documents/Masters_Jetson/Camera/devel/lib/Camera/Camera_node: /usr/lib/aarch64-linux-gnu/libopencv_dnn.so.4.1.1
/home/ros/Documents/Masters_Jetson/Camera/devel/lib/Camera/Camera_node: /usr/lib/aarch64-linux-gnu/libopencv_gapi.so.4.1.1
/home/ros/Documents/Masters_Jetson/Camera/devel/lib/Camera/Camera_node: /usr/lib/aarch64-linux-gnu/libopencv_highgui.so.4.1.1
/home/ros/Documents/Masters_Jetson/Camera/devel/lib/Camera/Camera_node: /usr/lib/aarch64-linux-gnu/libopencv_ml.so.4.1.1
/home/ros/Documents/Masters_Jetson/Camera/devel/lib/Camera/Camera_node: /usr/lib/aarch64-linux-gnu/libopencv_objdetect.so.4.1.1
/home/ros/Documents/Masters_Jetson/Camera/devel/lib/Camera/Camera_node: /usr/lib/aarch64-linux-gnu/libopencv_photo.so.4.1.1
/home/ros/Documents/Masters_Jetson/Camera/devel/lib/Camera/Camera_node: /usr/lib/aarch64-linux-gnu/libopencv_stitching.so.4.1.1
/home/ros/Documents/Masters_Jetson/Camera/devel/lib/Camera/Camera_node: /usr/lib/aarch64-linux-gnu/libopencv_video.so.4.1.1
/home/ros/Documents/Masters_Jetson/Camera/devel/lib/Camera/Camera_node: /usr/lib/aarch64-linux-gnu/libopencv_videoio.so.4.1.1
/home/ros/Documents/Masters_Jetson/Camera/devel/lib/Camera/Camera_node: /usr/lib/aarch64-linux-gnu/libopencv_imgcodecs.so.4.1.1
/home/ros/Documents/Masters_Jetson/Camera/devel/lib/Camera/Camera_node: /usr/lib/aarch64-linux-gnu/libopencv_calib3d.so.4.1.1
/home/ros/Documents/Masters_Jetson/Camera/devel/lib/Camera/Camera_node: /usr/lib/aarch64-linux-gnu/libopencv_features2d.so.4.1.1
/home/ros/Documents/Masters_Jetson/Camera/devel/lib/Camera/Camera_node: /usr/lib/aarch64-linux-gnu/libopencv_flann.so.4.1.1
/home/ros/Documents/Masters_Jetson/Camera/devel/lib/Camera/Camera_node: /usr/lib/aarch64-linux-gnu/libopencv_imgproc.so.4.1.1
/home/ros/Documents/Masters_Jetson/Camera/devel/lib/Camera/Camera_node: /usr/lib/aarch64-linux-gnu/libopencv_core.so.4.1.1
/home/ros/Documents/Masters_Jetson/Camera/devel/lib/Camera/Camera_node: Camera/CMakeFiles/Camera_node.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ros/Documents/Masters_Jetson/Camera/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable /home/ros/Documents/Masters_Jetson/Camera/devel/lib/Camera/Camera_node"
	cd /home/ros/Documents/Masters_Jetson/Camera/build/Camera && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Camera_node.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
Camera/CMakeFiles/Camera_node.dir/build: /home/ros/Documents/Masters_Jetson/Camera/devel/lib/Camera/Camera_node

.PHONY : Camera/CMakeFiles/Camera_node.dir/build

Camera/CMakeFiles/Camera_node.dir/requires: Camera/CMakeFiles/Camera_node.dir/src/camera_node.cpp.o.requires

.PHONY : Camera/CMakeFiles/Camera_node.dir/requires

Camera/CMakeFiles/Camera_node.dir/clean:
	cd /home/ros/Documents/Masters_Jetson/Camera/build/Camera && $(CMAKE_COMMAND) -P CMakeFiles/Camera_node.dir/cmake_clean.cmake
.PHONY : Camera/CMakeFiles/Camera_node.dir/clean

Camera/CMakeFiles/Camera_node.dir/depend:
	cd /home/ros/Documents/Masters_Jetson/Camera/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ros/Documents/Masters_Jetson/Camera/src /home/ros/Documents/Masters_Jetson/Camera/src/Camera /home/ros/Documents/Masters_Jetson/Camera/build /home/ros/Documents/Masters_Jetson/Camera/build/Camera /home/ros/Documents/Masters_Jetson/Camera/build/Camera/CMakeFiles/Camera_node.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : Camera/CMakeFiles/Camera_node.dir/depend

