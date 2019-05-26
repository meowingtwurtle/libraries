cmake_minimum_required(VERSION 3.10)
project(RandomCatLib)

cmake_policy(SET CMP0079 NEW)

add_library(__RC_AllLib INTERFACE)
add_library(RandomCat::AllLibraries ALIAS __RC_AllLib)

add_subdirectory(type_containers)
add_subdirectory(units)
add_subdirectory(util)