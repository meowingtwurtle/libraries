cmake_minimum_required(VERSION 3.10)
project(RandomCatUnits)

cmake_policy(SET CMP0079 NEW)

set(CMAKE_CXX_STANDARD 17)

add_library(__RC_Units INTERFACE)
target_include_directories(__RC_Units INTERFACE include)
target_link_libraries(__RC_Units INTERFACE RandomCat::TypeContainer)
add_library(RandomCat::Units ALIAS __RC_Units)

target_link_libraries(__RC_AllLib INTERFACE RandomCat::Units)
