find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIR})

link_directories(${caidas_SOURCE_DIR}/lib)

set(FUENTES_CVBLOB cvblob.cpp
                   cvlabel.cpp
)

set_source_files_properties(${FUENTES_SRC}
                            PROPERTIES
                            COMPILE_FLAGS "-O3"
)

add_library(cvblob ${FUENTES_CVBLOB})

install(TARGETS cvblob
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static
)