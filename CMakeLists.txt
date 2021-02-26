cmake_minimum_required(VERSION 3.19.2)
project(CppUmlClassGen CXX)

set(CMAKE_CXX_STANDARD 14)

find_package(Boost REQUIRED COMPONENTS filesystem program_options)

add_subdirectory(generator)
add_subdirectory(model)
add_subdirectory(util)
