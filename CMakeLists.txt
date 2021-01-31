cmake_minimum_required(VERSION 3.19.2)
project(CppUmlClassGen CXX)

# Common config stuff
include(Config.cmake)

# Utility functions
include(Functions.cmake)

add_subdirectory(generator)
add_subdirectory(model)
