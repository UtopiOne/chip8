cmake_minimum_required(VERSION 3.21)
project(chip
	VERSION 0.1
	LANGUAGES CXX
)

include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)

set(CMAKE_CXX_STANDART 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL3 REQUIRED)
find_package(fmt REQUIRED)

add_executable(chip
	src/Main.cpp
	src/Application.h
	src/Application.cpp
)
target_link_libraries(chip sdl::sdl fmt::fmt)
