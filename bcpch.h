#pragma once

#define _USE_MATH_DEFINES
#include <string>
#include <math.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <memory>
#include <format>
#include <unordered_map>
#include <array>

#include <glad/glad.h> 

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <GLFW/glfw3.h>

#include <stb_image.h>

#include "Texture.h"
#include "SubTexture2D.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Renderer2D.h"
#include "TextureManager.h"

template<typename... Args>
void log(std::string msg, Args... args)
{
	std::cout << std::format(msg, args...) << std::endl;
}
