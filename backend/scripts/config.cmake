# set C++11 compilation standard
set(CMAKE_CXX_STANDARD 11)

# Build type for cmake if it is release or debug.
if(CMAKE_BUILD_TYPE)
    if(CMAKE_BUILD_TYPE STREQUAL Release OR CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo OR CMAKE_BUILD_TYPE STREQUAL MinSizeRel)
        set(TARGET_POSTFIX "")
        set(APP_COMPILE_TYPE release)
    else()
        set(TARGET_POSTFIX "d")
        set(APP_COMPILE_TYPE debug)
    endif()
else()
    set(TARGET_POSTFIX "d")
    set(APP_COMPILE_TYPE debug)
endif()


# project version
set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 2)
set(PROJECT_VERSION_BUILD 0)

# Defining the name of the executable.
set(EXE_FILE_NAME "${PROJECT_NAME}_v${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_BUILD}${TARGET_POSTFIX}")

# How project static library to be named
set(LIBRARY_NAME ${PROJECT_NAME}_lib${TARGET_POSTFIX})

# This will provide verbose output from make
set(CMAKE_VERBOSE_MAKEFILE ON)

# install directory for the project, where binary would be created
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/install/")
