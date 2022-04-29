#include "../pch.h"
#include "Component.h"

Registry& Registry::get()
{
	static Registry registry;
	return registry;
}

entity Registry::create_entity()
{
	auto arr = std::array<IComponent*, NUM_COMPONENTS>();
	arr.fill(nullptr);
	m_entities.push_back(arr);
	return m_entities.size() - 1;
}

void Registry::add(entity e, IComponent* cmp)
{
	m_entities[e][static_cast<size_t>(cmp->T)] = cmp;
}

void Registry::remove(entity e, IComponent* cmp)
{
	m_entities[e][static_cast<size_t>(cmp->T)] = nullptr;
}

const std::array<IComponent*, NUM_COMPONENTS>& Registry::get_components(entity e) const
{
	return m_entities[e];
}

size_t Registry::size()
{
	return m_entities.size();
}
