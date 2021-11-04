cmake_minimum_required(VERSION 3.0.2)
project(stereo)

## Compile as C++11, supported in ROS Kinetic and newer
# add_compile_options(-std=c++11)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
)

find_path(ARGUS_INCLUDE_DIR Argus/Argus.h HINTS "/home/ros/Documents/Masters_Jetson/ws/src/stereo/external/argus/include")
find_path(ARGUS_INCLUDE_DIR ArgusHelpers.h HINTS "/home/ros/Documents/Masters_Jetson/ws/src/stereo/external/argus/samples/**")
set(ARGUS_DIR "/home/ros/Documents/Masters_Jetson/ws/src/stereo/external/argus/")
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "/home/ros/Documents/Masters_Jetson/ws/src/stereo/external/argus/cmake")
find_package(CUDA REQUIRED)
find_package(Argus REQUIRED)
find_package(OpenCV 4 REQUIRED)

message(STATUS "foo include dir: ${CUDA_DIR}")

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

add_library(utils_lib
  ${ARGUS_DIR}/samples/utils/ArgusHelpers.cpp
  ${ARGUS_DIR}/samples/utils/CUDAHelper.cpp
  ${ARGUS_DIR}/samples/utils/EGLGlobal.cpp
  ${ARGUS_DIR}/samples/utils/Observed.cpp
  ${ARGUS_DIR}/samples/utils/Thread.cpp
  ${ARGUS_DIR}/samples/utils/WindowBase.cpp
  ${ARGUS_DIR}/samples/utils/gtk/GuiElement.cpp
  ${ARGUS_DIR}/samples/utils/gtk/Window.cpp
)

target_link_libraries(utils_lib
  ${GTK3_LIBRARIES}
  ${OpenCV_LIBS}
)

target_include_directories(utils_lib
  PUBLIC
  ${ARGUS_DIR}/samples/utils
  ${ARGUS_DIR}/../include
  PRIVATE
  ${GTK3_INCLUDE_DIRS}
  
)

set(SOURCES src/video.cpp src/convert.cu)

include_directories(
  ${catkin_INCLUDE_DIRS}
  ${ARGUS_INCLUDE_DIR}
  ${CUDA_INCLUDE_DIRS}
  
)

catkin_package()
cuda_add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
  ${catkin_LIBRARIES}
  ${ARGUS_LIBRARIES}
  ${CUDA_LIBRARIES}
  ${OpenCV_LIBS}
  utils_lib
  cuda
)

include(CPack)