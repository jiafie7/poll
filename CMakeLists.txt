cmake_minimum_required(VERSION 3.19)

project(poll)

set(CMAKE_CXX_STANDARD 11)

add_compile_options(-g -O2 -Wall -Werror -Wno-unused)

include_directories(./)

file(GLOB_RECURSE SOURCES "utility/*.cpp" "socket/*.cpp" "task/*.cpp")

add_executable(server ${SOURCES} server.cpp)
add_executable(client ${SOURCES} client.cpp)
add_executable(poll_server ${SOURCES} poll_server.cpp)
add_executable(handler ${SOURCES} handler.cpp)
