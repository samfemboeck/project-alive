module;
#include <cstdint>
#include <vector>
#include <array>
export module ECS;

using entity = uint32_t;
#define NUM_COMPONENT_TYPES static_cast<size_t>(IComponent::Type::COUNT)

export struct IComponent
{
	enum class Type : size_t
	{
		RigidBody,
		CircleCollider,
		COUNT
	};

	bool IsAlive = false;
	Type T;
};

export class Registry
{
public:
	static Registry& get()
	{
		static Registry registry;
		return registry;
	}

	entity create_entity()
	{
		auto arr = std::array<const IComponent*, NUM_COMPONENT_TYPES>();
		arr.fill(nullptr);
		m_entities.push_back(arr);
		return m_entities.size() - 1;
	}

	void add(entity e, IComponent* cmp)
	{
		m_entities[e][static_cast<size_t>(cmp->T)] = cmp;
	}

	void remove(entity e, IComponent* cmp)
	{
		m_entities[e][static_cast<size_t>(cmp->T)] = nullptr;
	}

	auto& get_components(entity e) const
	{
		return m_entities[e];
	}

	size_t size()
	{
		return m_entities.size();
	}

private:
	std::vector<std::array<const IComponent*, NUM_COMPONENT_TYPES>> m_entities;
};

export template<typename T>
class ComponentSys
{
public:
	static ComponentSys<T>& get()
	{
		static ComponentSys<T> instance;
		return instance;
	}

	T& add_component(entity e)
	{
		Registry::get().add(e, &m_components[e]);
		m_components[e].IsAlive = true;
		return m_components[e];
	}

	void delete_component(entity e)
	{
		Registry::get().remove(e, &m_components[e]);
		m_components[e].IsAlive = false;
	}

	T* get_component(entity e)
	{
		return m_components[e].IsAlive ? &m_components[e] : nullptr;
	}

	void update(float deltaTime)
	{
	}

	auto& get_components() const
	{
		return m_components;
	}

protected:
	std::array<T, 200> m_components;

private:
	ComponentSys() = default;
};