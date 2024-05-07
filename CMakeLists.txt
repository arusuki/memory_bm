cmake_minimum_required(VERSION 3.23)

project(HelloWorld)

set(CMAKE_CXX_STANDARD 17)

add_executable(test_libcmalloc main.cpp)

add_executable(test_mimalloc main_mi.cpp)

find_package(mimalloc CONFIG REQUIRED)

target_link_libraries(test_mimalloc PRIVATE mimalloc-static)