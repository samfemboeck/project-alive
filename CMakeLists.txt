cmake_minimum_required (VERSION 3.15)

set(CMAKE_CXX_STANDARD 23)

project ("Alive")

add_executable(alive
	Main.cpp
	Shader.h
	Shader.cpp
	Buffer.h
	Buffer.cpp
	Camera.h
	Camera.cpp
	FrameBuffer.h
	FrameBuffer.cpp
	Renderer2D.h
	Renderer2D.cpp
	SubTexture2D.h
	SubTexture2D.cpp
	Texture.h
	Texture.cpp
	VertexArray.h
	VertexArray.cpp
	Organism.h
	Organism.cpp
	Cell.h
	Cell.cpp
	LeafCell.h
	LeafCell.cpp
	MoverCell.h
	MoverCell.cpp
	ThornCell.h
	ThornCell.cpp
	TextureManager.h
	TextureManager.cpp
	Transform.h
	Transform.cpp

	external/glad/include/glad/glad.h
	external/glad/src/glad.c
	external/stb/stb_image.h
)



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
