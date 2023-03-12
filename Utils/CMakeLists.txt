cmake_minimum_required(VERSION 3.24)
project(GMetalUtils)

set(CMAKE_CXX_STANDARD 20)

FILE(GLOB SRC
        "./*.cpp"
        "./include/*.cpp"
        "./include/*.h"
        "./src/*.cpp"
)

add_library(GMetalUtils STATIC ${SRC})
        
target_include_directories(GMetalUtils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
