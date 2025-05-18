# the minimum version of CMake.
cmake_minimum_required(VERSION 3.4.1)
project(MyApplication)

set(NATIVERENDER_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${NATIVERENDER_ROOT_PATH}
                    ${NATIVERENDER_ROOT_PATH}/include)

add_library(serialport SHARED serial.cpp)
target_link_libraries(serialport PUBLIC libace_napi.z.so libhilog_ndk.z.so)

add_library(SystemCpuUsage SHARED SystemCpuUsage.cpp)
target_link_libraries(SystemCpuUsage PUBLIC libace_napi.z.so libhilog_ndk.z.so)

add_library(burner SHARED burner.cpp)
target_link_libraries(burner PUBLIC libace_napi.z.so libhilog_ndk.z.so)