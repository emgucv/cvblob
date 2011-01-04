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
set(CVBLOB_VERSION_PATCH 3)
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
