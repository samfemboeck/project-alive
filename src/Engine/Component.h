#pragma once

using entity = uint32_t;

struct IComponent
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

#define NUM_COMPONENTS static_cast<size_t>(IComponent::Type::COUNT)

class Registry
{
public:
	static Registry& get();
	entity create_entity();
	void add(entity e, IComponent* cmp);
	void remove(entity e, IComponent* cmp);
	const std::array<IComponent*, NUM_COMPONENTS>& get_components(entity e) const;
	size_t size();

private:
	std::vector<std::array<IComponent*, NUM_COMPONENTS>> m_entities;
};

template<typename T>
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
