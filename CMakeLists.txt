cmake_minimum_required(VERSION 2.8)
project(sblog)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/sylar)

include (sylar/cmake/utils.cmake)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -fPIC")

include_directories(.)
include_directories(/apps/sylar/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/sylar)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/sylar/thirdpart)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/sylar/thirdpart)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/sylar/thirdpart/yaml-cpp/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/sylar/thirdpart/jsoncpp-1.8.4/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/orm_out)
link_directories(/apps/sylar/lib)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/orm_out)

find_package(Boost REQUIRED)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

#find_package(Protobuf REQUIRED)
#if(Protobuf_FOUND)
#    include_directories(${Protobuf_INCLUDE_DIRS})
#endif()
find_package(OpenSSL REQUIRED)
if(OPENSSL_FOUND)
    include_directories(${OPENSSL_INCLUDE_DIR})
endif()

set(LIB_SRC
        blog/my_module.cc
        blog/manager/user_manager.cc
        blog/servlets/user_create_servlet.cc
    )

#PROTOBUF_GENERATE_CPP(PB_SRCS PB_HDRS sylar/test.proto)
#list(APPEND LIB_SRC ${PB_SRCS})
#
#message(STATUS ${LIB_SRC})

add_library(sblog SHARED ${LIB_SRC})
add_dependencies(sblog liborm_data)
target_link_libraries(sblog orm_data)
force_redefine_file_macro_for_sources(sblog)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
