#   FindOpenCV.cmake
#   Copyright (C) 2007 by Cristóbal Carnero Liñán
#   grendel.ccl@gmail.com
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the
#   Free Software Foundation, Inc.,
#   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

include(UsePkgConfig OPTIONAL)

if (WIN32)
#if ("${CMAKE_SYSTEM_NAME}" MATCHES "Windows")
  message(STATUS "Windows detected.")
  if (CV_BASE)
    set(CV_BASE_INCLUDE ${CV_BASE}/include)
    set(CV_BASE_LIB ${CV_BASE}/lib)
  else (CV_BASE)
    set(CV_BASE_INCLUDE 
                        c:/OpenCV2.1/include
                        c:/Archivos de programa/OpenCV/cv/include
                        c:/Program Files/OpenCV/cv/include
                        c:/Archivos de programa/OpenCV/include/opencv
                        c:/Program Files/OpenCV/include/opencv
    )
    set(CV_BASE_LIB
                    c:/OpenCV2.1/lib
                    c:/Archivos de programa/OpenCV/lib
                    c:/Program Files/OpenCV/lib
    ) 
  endif (CV_BASE)

  find_path(OpenCV_cv_INCLUDE_DIR opencv/cv.h ${CV_BASE_INCLUDE})
  find_path(OpenCV_cxcore_INCLUDE_DIR opencv/cxcore.h ${CV_BASE_INCLUDE})
  find_path(OpenCV_highgui_INCLUDE_DIR opencv/highgui.h ${CV_BASE_INCLUDE})
  set(OpenCV_INCLUDE_DIR ${OpenCV_cv_INCLUDE_DIR} ${OpenCV_cxcore_INCLUDE_DIR} ${OpenCV_highgui_INCLUDE_DIR})

  #message(STATUS "CV_BASE_LIB: ${CV_BASE_LIB}")
  find_library(OpenCV_cv_LIBRARY NAMES cv210 cv PATH ${CV_BASE_LIB})
  find_library(OpenCV_cxcore_LIBRARY NAMES cxcore cxcore210 PATH ${CV_BASE_LIB})
  find_library(OpenCV_highgui_LIBRARY NAMES highgui highgui210 PATH ${CV_BASE_LIB})
  find_library(OpenCV_cvaux_LIBRARY NAMES cvaux cvaux210 PATH ${CV_BASE_LIB})
  #message(STATUS ${OpenCV_cv_LIBRARY})
  #message(STATUS ${OpenCV_cxcore_LIBRARY})
  #message(STATUS ${OpenCV_highgui_LIBRARY})
  #message(STATUS ${OpenCV_cvaux_LIBRARY})
else (WIN32)
#else ("${CMAKE_SYSTEM_NAME}" MATCHES "Windows")
  message(STATUS "Linux detected.")
  if (CV_BASE)
    set(CV_BASE_INCLUDE ${CV_BASE}/include)
    set(CV_BASE_LIB ${CV_BASE}/lib)
  else (CV_BASE)
    set(CV_BASE_INCLUDE 
                        /usr/include
                        /usr/local/include
    )
    set(CV_BASE_LIB
                    /usr/lib
                    /usr/local/lib
    ) 
  endif (CV_BASE)

  find_path(OpenCV_INCLUDE_DIR opencv/cv.h ${CV_BASE_INCLUDE})
  find_library(OpenCV_cv_LIBRARY NAMES cv cv0.9.7 cv0.9.8 cv1.0.0 PATH ${CV_BASE_LIB})
  find_library(OpenCV_cxcore_LIBRARY NAMES cxcore cxcore0.9.7 cxcore0.9.8 cxcore1.0.0 PATH ${CV_BASE_LIB})
  find_library(OpenCV_highgui_LIBRARY NAMES highgui highgui0.9.7 highgui0.9.8 highgui1.0.0 PATH ${CV_BASE_LIB})
  find_library(OpenCV_cvaux_LIBRARY NAMES cvaux cvaux0.9.7 cvaux0.9.8 cvaux1.0.0 PATH ${CV_BASE_LIB})
endif (WIN32)
#endif ("${CMAKE_SYSTEM_NAME}" MATCHES "Windows")

if (OpenCV_cv_LIBRARY AND OpenCV_cxcore_LIBRARY AND OpenCV_highgui_LIBRARY AND OpenCV_cvaux_LIBRARY)
  set(OpenCV_LIBRARIES ${OpenCV_cv_LIBRARY} ${OpenCV_cxcore_LIBRARY} ${OpenCV_highgui_LIBRARY} ${OpenCV_cvaux_LIBRARY})
  set(OpenCV_FOUND TRUE)
endif (OpenCV_cv_LIBRARY AND OpenCV_cxcore_LIBRARY AND OpenCV_highgui_LIBRARY AND OpenCV_cvaux_LIBRARY)

if (OpenCV_FOUND)
  if (NOT OpenCV_FIND_QUIETLY)
    message(STATUS "OpenCV found: ${OpenCV_LIBRARIES}.")
  endif (NOT OpenCV_FIND_QUIETLY)
else (OpenCV_FOUND)
  if (OpenCV_FIND_REQUIRED)
    message(FATAL_ERROR "OpenCV not found.")
  endif (OpenCV_FIND_REQUIRED)
endif (OpenCV_FOUND)
