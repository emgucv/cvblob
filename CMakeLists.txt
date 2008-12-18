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

# XXX It's possible that this can work with a previous version of CMake.
# TODO Check it.
cmake_minimum_required(VERSION 2.0)

project(cvBlob)

set(PACKAGE cvBlob)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeScripts)

#if(!WIN32)
#  set(CMAKE_EXE_LINKER_FLAGS -Bstatic)
#endif(!WIN32)

add_subdirectory(cvBlob lib)
add_subdirectory(test test)

#install(DIRECTORY dll DESTINATION dll)

if(WIN32)
  set(CPACK_GENERATOR "ZIP")
  set(CPACK_SOURCE_GENERATOR "ZIP")
else(WIN32)
  set(CPACK_GENERATOR "TGZ")
  set(CPACK_SOURCE_GENERATOR "TGZ")
endif(WIN32)

# Avoid "file cannot create directory: /opt/sbin. Maybe need administrative privileges."
#set(CPACK_SET_DESTDIR "ON")

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "OpenCV blob library")
set(CPACK_PACKAGE_CONTACT grendel.ccl@gmail.com)
set(CPACK_PACKAGE_VENDOR "Cristóbal Carnero Liñán <grendel.ccl@gmail.com>")
set(CPACK_PACKAGE_DESCRIPTION_FILE ${cvBlob_SOURCE_DIR}/README)
set(CPACK_PACKAGE_VERSION_MAJOR 0)
set(CPACK_PACKAGE_VERSION_MINOR 9)
set(CPACK_PACKAGE_VERSION_PATCH 4)
set(CPACK_PACKAGE_VERSION "0.9.4")
set(CPACK_RESOURCE_FILE_LICENSE ${cvBlob_SOURCE_DIR}/COPYING.LESSER)
set(CPACK_RESOURCE_FILE_README ${cvBlob_SOURCE_DIR}/README)

#set(CPACK_RESOURCE_FILE_WELCOME "/usr/share/CMake/Templates/CPack.GenericWelcome.txt")
#set(CPACK_PACKAGE_EXECUTABLES "${CMAKE_CURRENT_BINARY_DIR}/bin/cvBlob" "cvBlob")

set(CPACK_SOURCE_PACKAGE_FILE_NAME "cvBlob-src-${CPACK_PACKAGE_VERSION}")
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_IGNORE_FILES
    "~$"
    "/\\\\.svn/"
#     "^${PROJECT_SOURCE_DIR}/old/"
)

include(CPack)
