cmake_minimum_required(VERSION 3.24)
project(GMetal)

add_subdirectory(3rd/spdlog)
add_subdirectory(Utils)
add_subdirectory(Core)
add_subdirectory(App)

add_dependencies(GMetalCore GMetalUtils)
add_dependencies(GMetalApp GMetalCore)

