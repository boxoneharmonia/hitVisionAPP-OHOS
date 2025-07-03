# the minimum version of CMake.
cmake_minimum_required(VERSION 3.4.1)
project(MyApplication)

set(NATIVERENDER_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${NATIVERENDER_ROOT_PATH}
                    ${NATIVERENDER_ROOT_PATH}/include)

add_library(serialPortClass SHARED serialPortClass.cpp)
target_link_libraries(serialPortClass PUBLIC libhilog_ndk.z.so)

add_library(integrityCheckClass SHARED integrityCheckClass.cpp)
target_link_libraries(integrityCheckClass PUBLIC libhilog_ndk.z.so)

add_library(cpuUsage SHARED cpuUsage.cpp var.cpp)
target_link_libraries(cpuUsage PUBLIC libhilog_ndk.z.so)

add_library(integrityCheck1 SHARED integrityCheck1.cpp var.cpp)
target_link_libraries(integrityCheck1 PUBLIC libhilog_ndk.z.so integrityCheckClass)

add_library(serialPort1 SHARED serialPort1.cpp var.cpp)
target_link_libraries(serialPort1 PUBLIC serialPortClass)

add_library(service SHARED service.cpp)
target_link_libraries(service PUBLIC libhilog_ndk.z.so cpuUsage serialPort1 integrityCheck1 libace_napi.z.so)
