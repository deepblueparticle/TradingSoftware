include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${ODB_COMPILE_OUTPUT_DIR})

add_executable(runTests TestAlgomodelStrategy1.C
                        TestAlgomodelStrategy2.C
                        TestInfra.C
                        TestMain.C)

target_link_libraries(runTests ${LIBRARY_NAME}
                               ${SHARED_LIB_NAME}
                               ${Boost_LIBRARIES}
                               ${LIBEVENT_LIB}
                               ${LEVELDB_LIB}
                               ${QUICKFIX_LIB}
                               ${GTEST_LIB}
                               ${PLATFORM_SPECIFIC_LIBRARIES})

add_test(runTests runTests)
