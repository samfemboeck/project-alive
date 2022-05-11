cmake_minimum_required (VERSION 3.15)

set(CMAKE_CXX_STANDARD 23)

project ("Alive")

add_executable(alive
	src/Engine/Shader.h
	src/Engine/Shader.cpp
	src/Engine/Buffer.h
	src/Engine/Buffer.cpp
	src/Engine/Camera.h
	src/Engine/Camera.cpp
	src/Engine/FrameBuffer.h
	src/Engine/FrameBuffer.cpp
	src/Engine/Renderer2D.h
	src/Engine/Renderer2D.cpp
	src/Engine/SubTexture2D.h
	src/Engine/SubTexture2D.cpp
	src/Engine/Texture.h
	src/Engine/Texture.cpp
	src/Engine/VertexArray.h
	src/Engine/VertexArray.cpp
	src/Engine/TextureManager.h
	src/Engine/TextureManager.cpp
	src/Engine/Component.h
	src/Engine/Component.cpp
	src/Engine/Physics.h
	src/Engine/Physics.cpp
	src/Engine/QuickMaths.h
	src/Engine/QuickMaths.cpp
	src/Engine/OrthoCamController.h
	src/Engine/OrthoCamController.cpp
	src/Engine/Timer.h
	src/Engine/Timer.cpp
	src/Engine/Time.h
	src/Engine/App.h
	src/Engine/App.cpp

	src/Main.cpp
	src/Organism.h
	src/Organism.cpp
	src/Cell.h
	src/Cell.cpp
	src/LeafCell.h
	src/LeafCell.cpp
	src/MoverCell.h
	src/MoverCell.cpp
	src/ThornCell.h
	src/ThornCell.cpp
	src/LightCell.h
	src/LightCell.cpp
	src/ContactListener.h
	src/ContactListener.cpp

	external/glad/include/glad/glad.h
	external/glad/src/glad.c
	external/stb/stb_image.h
 )

add_subdirectory ("external/glfw")
add_subdirectory ("external/glm")
add_subdirectory("external/box2d")

target_precompile_headers(alive
	INTERFACE src/pch.h
	)

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
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/Shaders ${CMAKE_BINARY_DIR}/Testinger/Shaders
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/Textures ${CMAKE_BINARY_DIR}/Textures
	)
