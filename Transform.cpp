#include "bcpch.h"
#include "Transform.h"

glm::mat4 Transform::get_world()
{
	return Parent ? Parent->get_world() * get_local() : get_local();
}

glm::mat4 Transform::get_local()
{
	return glm::translate(glm::mat4(1.0f), { Position, 0 }) * glm::rotate(glm::mat4(1.0f), Rotation, { 0, 0, 1 }) * glm::scale(glm::mat4(1.0f), { Scale, 1 });
}
