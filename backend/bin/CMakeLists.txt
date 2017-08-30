#AlgoEngine executable
include_directories(${ODB_COMPILE_OUTPUT_DIR})
add_executable(${EXE_FILE_NAME} AlgoEngine.C)

set_target_properties("${EXE_FILE_NAME}" PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries("${EXE_FILE_NAME}" ${ODB_LIB_NAME}
                                         ${LIBRARY_NAME}
                                         ${SHARED_LIB_NAME}
                                         ${Boost_LIBRARIES}
                                         ${LIBEVENT_LIB}
                                         ${LEVELDB_LIB}
                                         ${QUICKFIX_LIB}
                                         ${IB_API_LIB}
                                         ${PLATFORM_SPECIFIC_LIBRARIES}
                                         "dl"
                                         "rt"
                                         "/usr/lib/x86_64-linux-gnu/libodb-boost.so"
                                         "/usr/lib/x86_64-linux-gnu/libodb-mysql.so"
                                         "/home/u/SourceCode/dependencies/odb-install/lib/libodb.so")
