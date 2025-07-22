cmake_minimum_required(VERSION 3.21)
project(chip
	VERSION 0.1
)

include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)

set(CMAKE_CXX_STANDART 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL3 REQUIRED)
find_package(glm REQUIRED)
find_package(spdlog REQUIRED)

add_library(glad
	vendor/glad/src/glad.c
)
target_include_directories(glad PUBLIC vendor/glad/include)

add_executable(chip
	src/Main.cpp

	src/Application.cpp
	src/Application.h

	src/Display.cpp
	src/Display.h

	src/Interpreter.cpp
	src/Interpreter.h

	src/Logging.cpp
	src/Logging.h
)
target_link_libraries(chip sdl::sdl spdlog::spdlog glm::glm glad)
