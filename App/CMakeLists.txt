cmake_minimum_required(VERSION 3.24)
project(GMetalApp)

set(CMAKE_CXX_STANDARD 20)

add_executable(GMetalApp main.cpp)

target_link_libraries(GMetalApp GMetalCore spdlog)