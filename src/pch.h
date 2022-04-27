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

#include "stb_image.h"

#include "Engine/Texture.h"
#include "Engine/SubTexture2D.h"
#include "Engine/VertexArray.h"
#include "Engine/Shader.h"
#include "Engine/Renderer2D.h"
#include "Engine/TextureManager.h"
#include "Engine/Transform.h"
#include "Engine/TickCountTimer.h"
#include "Engine/ClockTimer.h"
#include "Engine/Entity.h"

template<typename... Args>
void log(std::string msg, Args... args)
{
	std::cout << std::format(msg, args...) << std::endl;
}
