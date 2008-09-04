project(cvBlob)

set(PACKAGE cvBlob)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeScripts)

#if(!WIN32)
#  set(CMAKE_EXE_LINKER_FLAGS -Bstatic)
#endif(!WIN32)

add_subdirectory(cvBlob lib)
#add_subdirectory(test bin)

#install(DIRECTORY dll DESTINATION dll)
