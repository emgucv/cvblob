# Copyright (C) 2007 by Cristóbal Carnero Liñán
# grendel.ccl@gmail.com
#
# This file is part of cvBlob.
#
# cvBlob is free software: you can redistribute it and/or modify
# it under the terms of the Lesser GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# cvBlob is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# Lesser GNU General Public License for more details.
#
# You should have received a copy of the Lesser GNU General Public License
# along with cvBlob.  If not, see <http://www.gnu.org/licenses/>.
#

cmake_minimum_required(VERSION 2.0)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

project(cvBlob)

set(CVBLOB_VERSION_MAJOR 0)
set(CVBLOB_VERSION_MINOR 10)
set(CVBLOB_VERSION_PATCH 4)
set(CVBLOB_VERSION "${CVBLOB_VERSION_MAJOR}.${CVBLOB_VERSION_MINOR}.${CVBLOB_VERSION_PATCH}")

set(PACKAGE cvBlob)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeScripts)

set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(cvBlob lib)
#add_subdirectory(pyCvBlob pyCvBlob)
add_subdirectory(test test)

set(prefix  ${CMAKE_INSTALL_PREFIX})
set(version ${CVBLOB_VERSION})

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cvblob.pc.cmake.in" "${CMAKE_BINARY_DIR}/cvblob.pc" @ONLY IMMEDIATE)

# Save libs and executables in the same place
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib CACHE PATH "Output directory for libraries" )
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin CACHE PATH "Output directory for applications" )

# --------------------------------------------------------------------------------------------
#   Installation for CMake Module:  cvBlobConfig.cmake
#  Part 1/3: ${BIN_DIR}/cvBlobConfig.cmake              -> For use *without* "make install"
#  Part 2/3: ${BIN_DIR}/unix-install/cvBlobConfig.cmake -> For use with "make install"
#  Part 3/3: ${BIN_DIR}/win-install/cvBlobConfig.cmake  -> For use within binary installers/packages
# -------------------------------------------------------------------------------------------

# -------------------------------------------------------------------------------------------
#  Part 1/3: ${BIN_DIR}/cvBlobConfig.cmake              -> For use *without* "make install"
# -------------------------------------------------------------------------------------------

# Name of libraries is: libcv.so.1.1.0, etc...
#  CVBLOB_DLLVERSION must be set to "110", etc..
# Already done above.

# Set CMAKE_INCLUDE_DIRS_CONFIGCMAKE to the list of include directories:
set(CMAKE_INCLUDE_DIRS_CONFIGCMAKE "${CMAKE_CURRENT_SOURCE_DIR}/include/")
set(CMAKE_LIB_DIRS_CONFIGCMAKE "${LIBRARY_OUTPUT_PATH}")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cvBlobConfig.cmake.in" "${CMAKE_BINARY_DIR}/cvBlobConfig.cmake" IMMEDIATE @ONLY)

# --------------------------------------------------------------------------------------------
#  Part 2/3: ${BIN_DIR}/unix-install/cvBlobConfig.cmake -> For use *with* "make install"
# -------------------------------------------------------------------------------------------
# Set CMAKE_INCLUDE_DIRS_CONFIGCMAKE to the list of include directories:

set(CMAKE_INCLUDE_DIRS_CONFIGCMAKE "${CMAKE_INSTALL_PREFIX}/include/")
set(CMAKE_LIB_DIRS_CONFIGCMAKE "${CMAKE_INSTALL_PREFIX}/lib")

#exec_program(${CMAKE_COMMAND} ARGS "-E make_directory \"${CMAKE_BINARY_DIR}/unix-install/\"")
exec_program(mkdir ARGS "-p \"${CMAKE_BINARY_DIR}/unix-install/\"")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cvBlobConfig.cmake.in" "${CMAKE_BINARY_DIR}/unix-install/cvBlobConfig.cmake" IMMEDIATE @ONLY)

if(UNIX)
  # For a command "FIND_PACKAGE(FOO)", CMake will look at the directory /usr/share|lib/FOO/FOOConfig.cmake, so:
  install(FILES ${CMAKE_BINARY_DIR}/unix-install/cvBlobConfig.cmake DESTINATION ${CMAKE_INSTALL_PREFIX}/share/cvblob/)
endif()

# --------------------------------------------------------------------------------------------
#  Part 3/3: ${BIN_DIR}/win-install/cvBlobConfig.cmake  -> For use within binary installers/packages
# -------------------------------------------------------------------------------------------
if(WIN32)
  # Set CMAKE_INCLUDE_DIRS_CONFIGCMAKE to the list of include directories:

  # This will expand to, for example, <program files>/cvBlob 1.1.0/include
  set(CMAKE_INCLUDE_DIRS_CONFIGCMAKE "${THIS_CVBLOB_CONFIG_PATH}/include/")

  # This will expand to, for example, <program files>/cvBlob 1.1.0/lib
  set(CMAKE_LIB_DIRS_CONFIGCMAKE "\"\${THIS_CVBLOB_CONFIG_PATH}/lib\"")

  #exec_program(${CMAKE_COMMAND} ARGS "-E make_directory \"${CMAKE_BINARY_DIR}/win-install/\"")
  exec_program(mkdir ARGS "-p \"${CMAKE_BINARY_DIR}/win-install/\"")
  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cvBlobConfig.cmake.in" "${CMAKE_BINARY_DIR}/win-install/cvBlobConfig.cmake" IMMEDIATE @ONLY)
endif()
# --------------------------------------------------------------------------------------------

if(UNIX)
  install(FILES "${CMAKE_BINARY_DIR}/cvblob.pc" DESTINATION lib/pkgconfig)
endif(UNIX)

if(WIN32)
  set(CPACK_GENERATOR "ZIP")
  set(CPACK_SOURCE_GENERATOR "ZIP")
else(WIN32)
  set(CPACK_GENERATOR "TGZ")
  set(CPACK_SOURCE_GENERATOR "TGZ")
endif(WIN32)

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Blob library for OpenCV")
set(CPACK_PACKAGE_CONTACT grendel.ccl@gmail.com)
set(CPACK_PACKAGE_VENDOR "Cristóbal Carnero Liñán <grendel.ccl@gmail.com>")
set(CPACK_PACKAGE_DESCRIPTION_FILE ${cvBlob_SOURCE_DIR}/README)
set(CPACK_PACKAGE_VERSION_MAJOR ${CVBLOB_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${CVBLOB_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${CVBLOB_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION ${CVBLOB_VERSION})
set(CPACK_RESOURCE_FILE_LICENSE ${cvBlob_SOURCE_DIR}/COPYING.LESSER)
set(CPACK_RESOURCE_FILE_README ${cvBlob_SOURCE_DIR}/README)

set(CPACK_SOURCE_PACKAGE_FILE_NAME "cvBlob-src-${CPACK_PACKAGE_VERSION}")
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_IGNORE_FILES
  "~$"
  "/\\\\.svn/"
  #     "^${PROJECT_SOURCE_DIR}/old/"
)

include(CPack)
