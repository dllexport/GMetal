cmake_minimum_required(VERSION 3.24)
project(GMetal)

set(BOOST_ROOT "" CACHE STRING "")
set(VULKAN_SDK "" CACHE STRING "")

add_subdirectory(3rd/spdlog)
if(NOT WIN32)
    add_subdirectory(3rd/glfw-3.3.8)
endif()

add_subdirectory(Utils)
add_subdirectory(Core)
add_subdirectory(MeshExchange)
add_subdirectory(App)

add_dependencies(GMetalCore GMetalUtils)
add_dependencies(GMetalApp GMetalCore)
add_dependencies(GMetalMeshExchange GMetalCore)