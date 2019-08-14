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
        blog/word_parser.cc
        blog/index.cc
        blog/manager/article_manager.cc
        blog/manager/article_category_rel_manager.cc
        blog/manager/article_label_rel_manager.cc
        blog/manager/category_manager.cc
        blog/manager/comment_manager.cc
        blog/manager/label_manager.cc
        blog/manager/user_manager.cc
        blog/servlets/comment_create_servlet.cc
        blog/servlets/comment_delete_servlet.cc
        blog/servlets/comment_detail_servlet.cc
        blog/servlets/comment_verify_servlet.cc
        blog/servlets/comment_verify_list_servlet.cc
        blog/servlets/comment_query_servlet.cc
        blog/servlets/article_create_servlet.cc
        blog/servlets/article_delete_servlet.cc
        blog/servlets/article_detail_servlet.cc
        blog/servlets/article_nearby_servlet.cc
        blog/servlets/article_property_servlet.cc
        blog/servlets/article_publish_servlet.cc
        blog/servlets/article_query_servlet.cc
        blog/servlets/article_update_servlet.cc
        blog/servlets/article_update_category_servlet.cc
        blog/servlets/article_update_label_servlet.cc
        blog/servlets/article_snappy_servlet.cc
        blog/servlets/article_verify_list_servlet.cc
        blog/servlets/article_verify_servlet.cc
        blog/servlets/category_create_servlet.cc
        blog/servlets/category_delete_servlet.cc
        blog/servlets/category_query_servlet.cc
        blog/servlets/label_create_servlet.cc
        blog/servlets/label_delete_servlet.cc
        blog/servlets/label_query_servlet.cc
        blog/servlets/user_active_servlet.cc
        blog/servlets/user_create_servlet.cc
        blog/servlets/user_change_passwd_servlet.cc
        blog/servlets/user_exists_servlet.cc
        blog/servlets/user_forget_passwd_servlet.cc
        blog/servlets/user_info_servlet.cc
        blog/servlets/user_login_servlet.cc
        blog/servlets/user_logout_servlet.cc
        blog/servlets/user_query_servlet.cc
        blog/servlets/user_update_servlet.cc
        blog/struct.cc
        blog/util.cc
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
