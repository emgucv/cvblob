find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIR})
include_directories(${cvBlob_SOURCE_DIR}/cvBlob)

set(TEST_SRC test.cpp)

set_source_files_properties(${TEST_SRC}
                            PROPERTIES
                            COMPILE_FLAGS "-O3"
)

add_executable(test ${TEST_SRC})

target_link_libraries(test cvblob)
target_link_libraries(test ${OpenCV_LIBRARIES})

install(TARGETS test
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static
)
