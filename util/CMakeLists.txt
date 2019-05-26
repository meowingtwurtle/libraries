cmake_minimum_required(VERSION 3.10)
project(RandomCatUtil)

cmake_policy(SET CMP0079 NEW)

set(CMAKE_CXX_STANDARD 17)

add_library(__RC_Util INTERFACE)
target_include_directories(__RC_Util INTERFACE include)
add_library(RandomCat::Util ALIAS __RC_Util)

target_link_libraries(__RC_AllLib INTERFACE RandomCat::Util)
