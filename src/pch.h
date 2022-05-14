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
#include <unordered_map>
#include <vector>
#include <memory>
#include <format>
#include <unordered_map>
#include <array>
#include <functional>
#include <random>
#include <numbers>

#include <../../external/imgui/imgui.h>
#include "../../external/imgui/backends/imgui_impl_glfw.h"
#include "../../external/imgui/backends/imgui_impl_opengl3.h"

#include <glad/glad.h> 

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <GLFW/glfw3.h>

#include "stb_image.h"

#include <box2d/b2_math.h>
#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_circle_shape.h>
#include <box2d/b2_fixture.h>

#include "Engine/Texture.h"
#include "Engine/SubTexture2D.h"
#include "Engine/VertexArray.h"
#include "Engine/Shader.h"
#include "Engine/Renderer2D.h"
#include "Engine/TextureManager.h"
#include "Engine/Component.h"

#define LOG(...) std::cout << std::format(__VA_ARGS__) << std::endl

#define PROFILE(...) ScopeTimer timer(__VA_ARGS__)
