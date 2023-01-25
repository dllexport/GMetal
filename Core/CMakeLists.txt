cmake_minimum_required(VERSION 3.24)
project(GMetalCore)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost REQUIRED COMPONENTS system)

FILE(GLOB SRC
        "./*.cpp"
        "./include/*.cpp"
        "./src/*.cpp"
)

add_library(GMetalCore STATIC ${SRC})

target_include_directories(GMetalCore PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        $ENV{VULKAN_HEADERS}
        ${Boost_INCLUDE_DIRS}
)

target_link_libraries(GMetalCore PRIVATE
        $ENV{VULKAN_LIBRARIES}/vulkan-1.lib
        spdlog
        ${Boost_LIBRARIES}
        GMetalUtils
)

