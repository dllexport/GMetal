cmake_minimum_required(VERSION 3.24)
project(GMetalMeshExchange)

set(CMAKE_CXX_STANDARD 20)

FILE(GLOB SRC
        "./*.cpp"
        "./include/*.cpp"
        "./include/*.h"
        "./src/*.cpp"
)

find_package(assimp REQUIRED)

add_library(GMetalMeshExchange STATIC ${SRC})

target_include_directories(GMetalMeshExchange PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

target_link_libraries(GMetalMeshExchange PUBLIC 
    GMetalCore
    GMetalUtils
    assimp::assimp
)
