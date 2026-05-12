cmake_minimum_required(VERSION 3.27)

project(hyfakefocus VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)

add_library(hyfakefocus SHARED main.cpp)
find_package(PkgConfig REQUIRED)
pkg_check_modules(HYPRLAND REQUIRED IMPORTED_TARGET
    hyprland
    pixman-1
    libdrm
)

target_include_directories(hyfakefocus PRIVATE ${HYPRLAND_INCLUDE_DIRS})
target_link_libraries(hyfakefocus PRIVATE ${HYPRLAND_LIBRARIES})
target_compile_options(hyfakefocus PRIVATE ${HYPRLAND_CFLAGS_OTHER})
