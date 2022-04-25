#pragma once

class Transform
{
public:
	glm::vec2 pm_position = glm::vec2(0);
	float pm_rotation = 0;
	glm::vec2 pm_scale = glm::vec2(1);
	Transform* pm_parent = nullptr;
public:
	glm::mat4 get_world();
	glm::mat4 get_local();
};