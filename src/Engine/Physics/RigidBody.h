#pragma once
#include <vector>
#include <list>
#include "../Transform.h"

class Collider;
class IRigidBodyObserver;

class RigidBody
{
public:
	glm::vec2 pm_acceleration = glm::vec2(0);
	glm::vec2 pm_velocity = glm::vec2(0);
	std::string pm_layer;
	float pm_inv_mass = 1;
	std::list<glm::vec2> pm_forces;
	std::list<glm::vec2> pm_impulses;
	Transform pm_transform;
public:
	RigidBody();
	~RigidBody();
	void correct_position(float delta);
	void on_collision(Collider* other);
	void set_ignore(std::string layer, bool ignore);
	void set_ignore(RigidBody* rb, bool ignore);
	bool has_ignore(RigidBody* rb);
	void add_correction(const glm::vec2& correction);
private:
	glm::vec2 m_correction = glm::vec2(0);
	std::list<glm::vec2> m_forces;
	std::list<glm::vec2> m_impulses;
	std::map<std::string, bool> m_ignored_layers;
	std::map<RigidBody*, bool> m_ignored_rbs;
};