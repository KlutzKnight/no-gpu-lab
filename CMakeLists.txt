cmake_minimum_required(VERSION 3.26)

# if the generator is single config and build type is not defined,
# default to release build
get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(NOT is_multi_config AND NOT (CMAKE_BUILD_TYPE OR DEFINED ENV{CMAKE_BUILD_TYPE}))
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Release default")
endif()

project(nogpulab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(OpenMP COMPONENTS CXX)

set(SOURCES main.cpp model.cpp tgaimage.cpp gl.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/geometry)
target_link_libraries(${PROJECT_NAME} PRIVATE $<$<BOOL:${OpenMP_CXX_FOUND}>:OpenMP::OpenMP_CXX>)

file(GENERATE OUTPUT .gitignore CONTENT "*")
