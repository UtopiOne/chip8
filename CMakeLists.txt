cmake_minimum_required(VERSION 3.21)
project(chip
	VERSION 0.1
)

include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS "-pg")
set(CMAKE_EXE_LINKER_FLAGS "-pg")
set(CMAKE_SHARED_LINKER_FLAGS "-pg")

set(CMAKE_CXX_STANDART 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_definitions(LOGGING_ENABLED)

find_package(SDL3 REQUIRED)
find_package(glm REQUIRED)
find_package(spdlog REQUIRED)
find_package(imgui REQUIRED)

add_library(glad
	vendor/glad/src/glad.c
)
target_include_directories(glad PUBLIC vendor/glad/include)

add_executable(chip
	src/Main.cpp

	src/Application.cpp
	src/Application.h

	src/Audio.cpp
	src/Audio.h

	src/Display.cpp
	src/Display.h

	src/Interpreter.cpp
	src/Interpreter.h

	src/Keycodes.h

	src/Logging.cpp
	src/Logging.h

	src/Shader.cpp
	src/Shader.h

	vendor/imgui/imgui_impl_sdl3.cpp
	vendor/imgui/imgui_impl_sdl3.h
	vendor/imgui/imgui_impl_opengl3.cpp
	vendor/imgui/imgui_impl_opengl3.h
	vendor/imgui/imgui_impl_opengl3_loader.h
)

target_include_directories(chip PUBLIC vendor/imgui)
target_link_libraries(chip sdl::sdl spdlog::spdlog glm::glm imgui::imgui glad)
