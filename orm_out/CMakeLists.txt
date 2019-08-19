cmake_minimum_required(VERSION 3.0)
project(orm_data)

set(LIB_SRC
    blog/data/article_info.cc
    blog/data/article_category_rel_info.cc
    blog/data/article_label_rel_info.cc
    blog/data/category_info.cc
    blog/data/channel_info.cc
    blog/data/comment_info.cc
    blog/data/label_info.cc
    blog/data/user_info.cc
)
add_library(orm_data ${LIB_SRC})
force_redefine_file_macro_for_sources(orm_data)
