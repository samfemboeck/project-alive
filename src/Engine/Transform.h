#pragma once

struct Transform
{
	glm::vec2 Position = glm::vec2(0);
	float Rotation = 0;
	glm::vec2 Scale = glm::vec2(1);
	Transform* Parent = nullptr;

	glm::mat4 get_world();
	glm::mat4 get_local();
};