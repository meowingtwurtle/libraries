cmake_minimum_required(VERSION 3.10)
project(RandomCatTypeContainer)

cmake_policy(SET CMP0079 NEW)

set(CMAKE_CXX_STANDARD 17)

add_library(__RC_TypeContainer INTERFACE)
target_include_directories(__RC_TypeContainer INTERFACE include)
add_library(RandomCat::TypeContainer ALIAS __RC_TypeContainer)

target_link_libraries(__RC_AllLib INTERFACE RandomCat::TypeContainer)
