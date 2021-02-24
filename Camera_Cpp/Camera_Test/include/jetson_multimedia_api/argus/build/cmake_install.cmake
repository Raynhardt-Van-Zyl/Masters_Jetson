# Install script for directory: /usr/src/jetson_multimedia_api/argus

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/usr/src/jetson_multimedia_api/argus/build/apps/camera/cmake_install.cmake")
  include("/usr/src/jetson_multimedia_api/argus/build/samples/bayerAverageMap/cmake_install.cmake")
  include("/usr/src/jetson_multimedia_api/argus/build/samples/cudaBayerDemosaic/cmake_install.cmake")
  include("/usr/src/jetson_multimedia_api/argus/build/samples/cudaHistogram/cmake_install.cmake")
  include("/usr/src/jetson_multimedia_api/argus/build/samples/denoise/cmake_install.cmake")
  include("/usr/src/jetson_multimedia_api/argus/build/samples/eglImage/cmake_install.cmake")
  include("/usr/src/jetson_multimedia_api/argus/build/samples/gstVideoEncode/cmake_install.cmake")
  include("/usr/src/jetson_multimedia_api/argus/build/samples/histogram/cmake_install.cmake")
  include("/usr/src/jetson_multimedia_api/argus/build/samples/multiSensor/cmake_install.cmake")
  include("/usr/src/jetson_multimedia_api/argus/build/samples/multiStream/cmake_install.cmake")
  include("/usr/src/jetson_multimedia_api/argus/build/samples/oneShot/cmake_install.cmake")
  include("/usr/src/jetson_multimedia_api/argus/build/samples/openglBox/cmake_install.cmake")
  include("/usr/src/jetson_multimedia_api/argus/build/samples/syncSensor/cmake_install.cmake")
  include("/usr/src/jetson_multimedia_api/argus/build/samples/syncStereo/cmake_install.cmake")
  include("/usr/src/jetson_multimedia_api/argus/build/samples/yuvJpeg/cmake_install.cmake")
  include("/usr/src/jetson_multimedia_api/argus/build/samples/userAutoExposure/cmake_install.cmake")
  include("/usr/src/jetson_multimedia_api/argus/build/samples/userAutoWhiteBalance/cmake_install.cmake")
  include("/usr/src/jetson_multimedia_api/argus/build/samples/utils/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/usr/src/jetson_multimedia_api/argus/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
