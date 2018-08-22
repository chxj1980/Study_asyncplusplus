cmake_minimum_required (VERSION 2.6)

INCLUDE_DIRECTORIES(../../thirdparty/comm)

FIND_LIBRARY(COMM_LIB comm ../../thirdparty/comm/lib NO_DEFAULT_PATH)
FIND_LIBRARY(RUNTIME_LIB rt /usr/lib  /usr/local/lib NO_DEFAULT_PATH)

link_libraries(${COMM_LIB} ${RUNTIME_LIB})

ADD_DEFINITIONS(
-O3 -g -W -Wall
 -Wunused-variable -Wunused-parameter -Wunused-function -Wunused
 -Wno-deprecated -Woverloaded-virtual -Wwrite-strings
 -D__WUR= -D_REENTRANT -D_FILE_OFFSET_BITS=64 -DTIXML_USE_STL
)


add_library(lib_demo
        cmd.cpp
        global.cpp
        md5.cpp
)

link_libraries(lib_demo)
add_executable(demo
        main.cpp
)
LINK_DIRECTORIES(${XX1_LIB_PATH} ${XX2_LIB_PATH})

# link library in static mode
target_link_libraries(demo libuuid.a)


target_link_libraries(${PROJECT_NAME}
  ws2_32.lib
  psapi.lib
  iphlpapi.lib
  debug  ${CMAKE_CURRENT_LIST_DIR}/../../libuv/Debug/lib/libuv.lib
  optimized   ${CMAKE_CURRENT_LIST_DIR}/../../libuv/Release/lib/libuv.lib
)


# 添加依赖库
find_library(LIB_I_NEED boost_regex-vc120-mt-gd-1_55 C:/boost_1_55_0/lib32-msvc-12.0)
if(${LIB_I_NEED} STREQUAL "LIB_I_NEED-NOTFOUND")
    message(FATAL_ERROR "Couldn't find the 'external_lib' library")
endif()

message(STATUS "Found 'external_lib' at: ${LIB_I_NEED}")

# ${PROJECT_NAME} 不能省略
target_link_libraries (${PROJECT_NAME} ${LIB_I_NEED})