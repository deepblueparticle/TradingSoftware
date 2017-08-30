# ODB generated files
# set files to process with ODB
set(DB_ENTITIES_FOLDER_PATH ${CMAKE_SOURCE_DIR}/include/db)
file(GLOB_RECURSE OdbCMake_ODB_HEADERS ${DB_ENTITIES_FOLDER_PATH}/*.H)

# set ODB include directories, flags and compile persistent classes with ODB
set(ODB_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/../shared/include)
odb_compile(ODB_GENERATED_FILES FILES ${OdbCMake_ODB_HEADERS} INCLUDE ${ODB_INCLUDE_DIRS} INCLUDE_PREFIX db STANDARD c++11 DB mysql GENERATE_QUERY GENERATE_SESSION GENERATE_SCHEMA)

# Add sources
MACRO(ADDSRC result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(SRCFILES "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
      FILE(GLOB MyCSources ${curdir}/${child}/*.C)
      LIST(APPEND SRCFILES ${MyCSources})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${SRCFILES})
ENDMACRO()

ADDSRC(CPPSRCFILES ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${ODB_COMPILE_OUTPUT_DIR})
add_library(${LIBRARY_NAME} ${CPPSRCFILES} ${ODB_GENERATED_FILES})
