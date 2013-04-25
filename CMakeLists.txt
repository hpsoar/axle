set(proj_name axle)

cmake_minimum_required(VERSION 2.8)

include(CMakeLists-base.txt)

include_directories(./include 
                    ${magic_dir}/glm-0.9.3.B)

add_subdirectory(src)
