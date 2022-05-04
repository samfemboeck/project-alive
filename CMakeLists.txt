cmake_minimum_required (VERSION 3.15)

set(CMAKE_CXX_STANDARD 23)

project ("Alive")

add_executable(alive
	src/Main.cpp
	external/glad/include/glad/glad.h
	external/glad/src/glad.c
	external/stb/stb_image.h
   "src/Organism.ixx"  "src/Engine/ECS.ixx" "src/Engine/QuickMaths.ixx" "src/Cell.ixx" "src/Engine/Physics.ixx" "src/Engine/Timer.ixx" "src/Engine/Renderer.ixx" "src/Engine/VertexArray.ixx" "src/Engine/Buffer.ixx" "src/Engine/Texture.ixx" "src/Engine/Shader.ixx" "src/Engine/Camera.ixx" "src/Engine/FrameBuffer.ixx" "src/Engine/App.ixx")

set_property(TARGET alive PROPERTY CXX_STANDARD 20)

add_subdirectory ("external/glfw")
add_subdirectory ("external/glm")
add_subdirectory("external/box2d")

target_include_directories (alive
	PUBLIC ${PROJECT_SOURCE_DIR}/external/glad/include
	PUBLIC ${PROJECT_SOURCE_DIR}/external/glfw/include
	PUBLIC ${PROJECT_SOURCE_DIR}/external/glm
	PUBLIC ${PROJECT_SOURCE_DIR}/external/stb
	PUBLIC ${PROJECT_SOURCE_DIR}/external/box2d/include
	)

target_link_libraries (
	alive
	glfw
	glm
	box2d
)

add_custom_command(TARGET alive POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/Shaders ${CMAKE_BINARY_DIR}/Shaders
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/Textures ${CMAKE_BINARY_DIR}/Textures
	)
