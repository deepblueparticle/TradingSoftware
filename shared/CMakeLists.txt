include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

#EasyLogging library will crash if first two are not defined. https://github.com/easylogging/easyloggingpp/issues/391
add_definitions(-DELPP_THREADING_ENABLED=1 -DELPP_THREAD_SAFE -D_CRT_SECURE_NO_WARNINGS -DELPP_DISABLE_DEFAULT_CRASH_HANDLING)

# currently only these files being built. We need to revisit sharedLib later.
add_library("sharedLib${TARGET_POSTFIX}" ${CMAKE_CURRENT_SOURCE_DIR}/src/shared/time_util.cpp
                               ${CMAKE_CURRENT_SOURCE_DIR}/src/shared/commands.cpp
                               ${CMAKE_CURRENT_SOURCE_DIR}/src/shared/defines.cpp
                               ${CMAKE_CURRENT_SOURCE_DIR}/src/shared/response.cpp
                               ${CMAKE_CURRENT_SOURCE_DIR}/src/shared/order.cpp
                               ${CMAKE_CURRENT_SOURCE_DIR}/src/shared/iophandler.cpp)

#export the sharedLib name outside so that backend and backtester can use
set(SHARED_LIB_NAME "sharedLib${TARGET_POSTFIX}" PARENT_SCOPE)
