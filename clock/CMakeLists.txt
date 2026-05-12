cmake_minimum_required(VERSION 3.27)

project(hyclock VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)

add_library(hyclock SHARED main.cpp ClockOverlay.hpp ClockOverlay.cpp ClockPassElement.hpp ClockPassElement.cpp)
find_package(PkgConfig REQUIRED)
pkg_check_modules(HYPRLAND REQUIRED IMPORTED_TARGET
    hyprland
    pixman-1
    libdrm
    pango
)

target_include_directories(hyclock PRIVATE ${HYPRLAND_INCLUDE_DIRS})
target_link_libraries(hyclock PRIVATE ${HYPRLAND_LIBRARIES})
target_compile_options(hyclock PRIVATE ${HYPRLAND_CFLAGS_OTHER})
