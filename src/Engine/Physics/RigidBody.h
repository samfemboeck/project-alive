#pragma once
#include <vector>
#include <list>
#include "../Transform.h"
#include "../IComponent.h"

struct Collider;

struct RigidBody : IComponent
{
	RigidBody* Parent = nullptr;
	glm::vec2 Acceleration = glm::vec2(0);
	glm::vec2 Velocity = glm::vec2(0);
	std::string Layer;
	float InvMass = 1;
	std::vector<glm::vec2> Forces;
	std::vector<glm::vec2> Impulses;
	Transform Transform;
	glm::vec2 Correction = glm::vec2(0);
	std::map<std::string, bool> IgnoredLayers;
	std::map<RigidBody*, bool> IgnoredRBs;

	RigidBody();
	~RigidBody();
	void correct_position(float delta);
	void on_collision(Collider* other);
	void set_ignore(std::string layer, bool ignore);
	void set_ignore(RigidBody* rb, bool ignore);
	bool has_ignore(RigidBody* rb);
	void add_correction(const glm::vec2& correction);
	void update() override {}
};

template<>
void ComponentSys<RigidBody>::update();


