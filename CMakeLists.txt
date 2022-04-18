cmake_minimum_required (VERSION 3.15)

project ("Alive")

add_executable(alive
	Main.cpp
	Shader.h
	Shader.cpp

	external/glad/include/glad/glad.h
	external/glad/src/glad.c
	external/stb/stb_image.h
	)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory ("external/glfw")
add_subdirectory ("external/glm")

target_precompile_headers(alive
	INTERFACE pch.h
	)

target_include_directories (alive
	PUBLIC ${PROJECT_SOURCE_DIR}/external/glad/include
	PUBLIC ${PROJECT_SOURCE_DIR}/external/glfw/include
	PUBLIC ${PROJECT_SOURCE_DIR}/external/glm
	PUBLIC ${PROJECT_SOURCE_DIR}/external/stb
	)

target_link_libraries (
	alive
	glfw
	glm
)

add_custom_command(TARGET alive POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/Shaders ${CMAKE_BINARY_DIR}/Shaders
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/Textures ${CMAKE_BINARY_DIR}/Textures
	)
