#### Add all dependent libraries path here. These values to be changed by user ####
set(BOOST_ROOT "/home/u/SourceCode/dependencies/boost-install/")
set(QUICKFIX_ROOT "/home/u/SourceCode/dependencies/quickfix-install")
set(LEVELDB_ROOT "/home/u/SourceCode/dependencies/leveldb/")
set(LIBEVENT_ROOT "/home/u/SourceCode/dependencies/libevent-install/")
SET(GTEST_ROOT "/home/u/SourceCode/dependencies/gtest/")
SET(IB_API_ROOT "/home/u/SourceCode/dependencies/ib_api")
SET(NANEX_ROOT "/home/ubuntu/Algoengine/dependencies/nanex/Nanex_Prod/install")

#### Boost library dependency ####
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(Boost_NO_SYSTEM_PATHS ON)

if(POLICY CMP0020)
    cmake_policy(SET CMP0020 NEW)
endif()

find_package(Boost REQUIRED COMPONENTS system filesystem program_options date_time serialization)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
else()
    print_error ("Please make sure you have Boost Installed" "Boost at ${BOOST_ROOT}")
endif ()

#### QuickFix library dependency ####
include_directories(${QUICKFIX_ROOT}/include)
include_directories(${QUICKFIX_ROOT}/include/quickfix) # This is needed for shared lib
set(QUICKFIX_LIB_PATH "${QUICKFIX_ROOT}/lib")

#### LevelDb library dependency ####
include_directories(${LEVELDB_ROOT}/include)
set(LEVELDB_LIB_PATH "${LEVELDB_ROOT}/lib")

#### LibEvent library dependency ####
include_directories(${LIBEVENT_ROOT}/include)
set(LIBEVENT_LIB_PATH "${LIBEVENT_ROOT}/lib")

#### Google test library dependency ####
include_directories(${GTEST_ROOT}/include)
set(GTEST_LIB_PATH "${GTEST_ROOT}/lib")

#### Interactive Brokers API library dependency ####
include_directories(${IB_API_ROOT}/include)
set(IB_API_LIB_PATH "${IB_API_ROOT}/lib")

#### Nanex API library dependency ####
include_directories(${NANEX_ROOT}/include)
set(NANEX_LIB_PATH "${NANEX_ROOT}/lib")

#### System library dependency ####
set(PLATFORM_SPECIFIC_LIBRARIES pthread)

#### Finally add cutomer library path in CMake library path and find them to be added in linker call ####
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH}
    ${LEVELDB_LIB_PATH} ${LIBEVENT_LIB_PATH} ${QUICKFIX_LIB_PATH} ${GTEST_LIB_PATH} ${IB_API_LIB_PATH}
)
find_library(QUICKFIX_LIB quickfix)
find_library(LEVELDB_LIB leveldb)
find_library(LIBEVENT_LIB event)
find_library(GTEST_LIB gtest)
find_library(IB_API_LIB ib_api)

#### Add AlgoEngine shared directory library dependency ####
add_subdirectory(${CMAKE_SOURCE_DIR}/../shared/ ${CMAKE_CURRENT_BINARY_DIR}/shared)
include_directories(${CMAKE_SOURCE_DIR}/../shared/include)

#EasyLogging library will crash if first two are not defined. https://github.com/easylogging/easyloggingpp/issues/391
add_definitions(-DELPP_THREADING_ENABLED=1 -DELPP_THREAD_SAFE -D_CRT_SECURE_NO_WARNINGS -DELPP_DISABLE_DEFAULT_CRASH_HANDLING)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules")
find_package(ODB REQUIRED COMPONENTS mysql)
include(${ODB_USE_FILE})

configure_file (
  "${CMAKE_SOURCE_DIR}/include/BackendConfig.H.in"
  "${CMAKE_BINARY_DIR}/BackendConfig.H"
)
include_directories(${CMAKE_BINARY_DIR})
