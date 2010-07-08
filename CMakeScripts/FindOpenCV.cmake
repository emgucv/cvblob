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
find_path(OpenCV_cv_INCLUDE_DIR opencv/cv.h
	c:/Archivos de programa/OpenCV/cv/include
	c:/Program Files/OpenCV/cv/include
	c:/Archivos de programa/OpenCV/include/opencv
	c:/Program Files/OpenCV/include/opencv
)

find_path(OpenCV_cxcore_INCLUDE_DIR opencv/cxcore.h
	c:/Archivos de programa/OpenCV/cxcore/include
	c:/Program Files/OpenCV/cxcore/include
	c:/Archivos de programa/OpenCV/include/opencv
	c:/Program Files/OpenCV/include/opencv
)


find_path(OpenCV_highgui_INCLUDE_DIR opencv/highgui.h
	c:/Archivos de programa/OpenCV/otherlibs/highgui
	c:/Program Files/OpenCV/otherlibs/highgui
	c:/Archivos de programa/OpenCV/include/opencv
	c:/Program Files/OpenCV/include/opencv
)

set(OpenCV_INCLUDE_DIR $(OpenCV_cv_INCLUDE_DIR) $(OpenCV_cxcore_INCLUDE_DIR) $(OpenCV_highgui_INCLUDE_DIR)) 

find_library(OpenCV_cv_LIBRARY
              NAMES cv
              PATH c:/Archivos de programa/OpenCV/lib
                   c:/Program Files/OpenCV/lib
)

find_library(OpenCV_cxcore_LIBRARY
              NAMES cxcore
              PATH c:/Archivos de programa/OpenCV/lib
                   c:/Program Files/OpenCV/lib
)

find_library(OpenCV_highgui_LIBRARY
              NAMES highgui
              PATH c:/Archivos de programa/OpenCV/lib
                   c:/Program Files/OpenCV/lib
)

find_library(OpenCV_cvaux_LIBRARY
              NAMES cvaux
              PATH c:/Archivos de programa/OpenCV/lib
                   c:/Program Files/OpenCV/lib
)
else (WIN32)
find_path(OpenCV_INCLUDE_DIR opencv/cv.h
          /usr/include
          /usr/local/include
)

find_library(OpenCV_cv_LIBRARY
              NAMES cv cv0.9.7 cv0.9.8 cv1.0.0
              PATH /usr/lib
                   /usr/local/lib
)

find_library(OpenCV_cxcore_LIBRARY
              NAMES cxcore cxcore0.9.7 cxcore0.9.8 cxcore1.0.0
              PATH /usr/lib
                   /usr/local/lib
)

find_library(OpenCV_highgui_LIBRARY
              NAMES highgui highgui0.9.7 highgui0.9.8 highgui1.0.0
              PATH /usr/lib
                   /usr/local/lib
)

find_library(OpenCV_cvaux_LIBRARY
              NAMES cvaux cvaux0.9.7 cvaux0.9.8 cvaux1.0.0
              PATH /usr/lib
                   /usr/local/lib
)
endif (WIN32)

if (OpenCV_cv_LIBRARY AND OpenCV_cxcore_LIBRARY AND OpenCV_highgui_LIBRARY AND OpenCV_cvaux_LIBRARY)
  set(OpenCV_LIBRARIES ${OpenCV_cv_LIBRARY} ${OpenCV_cxcore_LIBRARY} ${OpenCV_highgui_LIBRARY} ${OpenCV_cvaux_LIBRARY})
  set(OpenCV_FOUND TRUE)
endif (OpenCV_cv_LIBRARY AND OpenCV_cxcore_LIBRARY AND OpenCV_highgui_LIBRARY AND OpenCV_cvaux_LIBRARY)

if (OpenCV_FOUND)
  if (NOT OpenCV_FIND_QUIETLY)
    message(STATUS "OpenCV encontrado: ${OpenCV_LIBRARIES}")
  endif (NOT OpenCV_FIND_QUIETLY)
else (OpenCV_FOUND)
  if (OpenCV_FIND_REQUIRED)
    message(FATAL_ERROR "No se puede encontrar OpenCV")
  endif (OpenCV_FIND_REQUIRED)
endif (OpenCV_FOUND)
