cmake_minimum_required(VERSION 3.24)
project(GMetalCore)

set(CMAKE_CXX_STANDARD 20)
set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/Bin/glslangValidator.exe")

find_package(Boost REQUIRED COMPONENTS system)

FILE(GLOB SRC
        "./*.cpp"
        "./include/*.cpp"
        "./src/*.cpp"
)

file(GLOB_RECURSE GLSL_SOURCE_FILES
    "shaders/*.vert"
    "shaders/*.frag"
)

foreach(GLSL ${GLSL_SOURCE_FILES})
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${PROJECT_BINARY_DIR}/shaders/${FILE_NAME}.spv")
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/shaders/"
    COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    Shaders 
    DEPENDS ${SPIRV_BINARY_FILES}
)

add_library(GMetalCore STATIC ${SRC})

add_dependencies(GMetalCore Shaders)

target_include_directories(GMetalCore PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        $ENV{VULKAN_SDK}/Include
        ${Boost_INCLUDE_DIRS}
)

target_link_libraries(GMetalCore PRIVATE
        $ENV{VULKAN_SDK}/Lib/vulkan-1.lib
        spdlog
        ${Boost_LIBRARIES}
        GMetalUtils
)

