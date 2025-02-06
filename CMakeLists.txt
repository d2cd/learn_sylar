cmake_minimum_required(VERSION 2.8)
project(sylar)

# include (cmake/utils.cmake) //这个文件重定义__FILE__宏为相对路径，但是还没写

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")

set(LIB_SRC 
    sylar/log.cc
    sylar/util.cc
    sylar/config.cc
    sylar/thread.cc
   )
add_library(sylar SHARED ${LIB_SRC})

include_directories(${CMAKE_SOURCE_DIR}/sylar)
#include中包含yaml-cpp
include_directories(/usr/local/include)


find_package(yaml-cpp REQUIRED)
find_package(Boost REQUIRED COMPONENTS thread)
find_package(Threads REQUIRED)

set (LIBS 
        sylar 
        yaml-cpp 
        Threads::Threads
    )

add_executable(test sylar/tests/test.cc)
#相对地址是相对于cmakelist文件所在的目录
add_dependencies(test sylar)
target_link_libraries(test ${LIBS})


add_executable(test_config sylar/tests/test_config.cc)
#相对地址是相对于cmakelist文件所在的目录
add_dependencies(test_config sylar)
#test_config虽然没有用到pthread，但是链接的动态库中用到了pthread
target_link_libraries(test_config ${LIBS})

add_executable(test_thread sylar/tests/test_thread.cc)
#相对地址是相对于cmakelist文件所在的目录
add_dependencies(test_thread sylar)
target_link_libraries(test_thread ${LIBS})

SET (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET (LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

