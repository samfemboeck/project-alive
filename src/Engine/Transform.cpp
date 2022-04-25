#include "../pch.h"
#include "Transform.h"

glm::mat4 Transform::get_world()
{
	return pm_parent ? pm_parent->get_world() * get_local() : get_local();
}

glm::mat4 Transform::get_local()
{
	return glm::translate(glm::mat4(1.0f), { pm_position, 0 }) * glm::rotate(glm::mat4(1.0f), pm_rotation, { 0, 0, 1 }) * glm::scale(glm::mat4(1.0f), { pm_scale, 1 });
}
