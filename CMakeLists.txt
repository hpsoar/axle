set(proj_name axle)

cmake_minimum_required(VERSION 2.8)

include(CMakeLists-base.txt)

include_directories(${magic_dir}/glm-0.9.4.3)

if (APPLE)
include_directories(/opt/local/include)
endif ()

add_definitions(-std=c++0x)

add_subdirectory(src)
