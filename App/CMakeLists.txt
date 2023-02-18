cmake_minimum_required(VERSION 3.24)
project(GMetalApp)

set(CMAKE_CXX_STANDARD 20)

add_executable(GMetalApp main.cpp mainMacOS.cpp)

target_link_libraries(GMetalApp GMetalCore spdlog)

# copy spv from Core to App build dst
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${PROJECT_BINARY_DIR}/../Core/shaders
                ${CMAKE_CURRENT_BINARY_DIR}/shaders)