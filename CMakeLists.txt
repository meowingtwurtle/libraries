cmake_minimum_required(VERSION 3.10)
project(RandomCatLib)

cmake_policy(SET CMP0079 NEW)

add_library(__RC_All INTERFACE)
add_library(RandomCat::All ALIAS __RC_All)
