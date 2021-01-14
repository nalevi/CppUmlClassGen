cmake_minimum_required(VERSION 3.19.2)
project(CppUmlClassGen)

# Common config stuff
include(Config.cmake)

# Utility functions
include(Functions.cmake)

add_subdirectory(generator)

