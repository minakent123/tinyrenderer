cmake_minimum_required(VERSION 3.9)
project(tinyrenderer CXX)

set(TARGET tinyrenderer)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

get_srcs("${CMAKE_CURRENT_SOURCE_DIR}" SRCS)

add_executable(${TARGET} ${SRCS})
add_dependencies(${TARGET} prebuild_scripts)
set_compile_options(${TARGET})
set_common_compile_definitions(${TARGET})
