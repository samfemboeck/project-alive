#pragma once

struct IComponent
{
	enum class Type
	{
		RigidBody = 0,
		COUNT = 1
	};

	Type T;
	virtual void update() = 0;
};

class Registry
{
public:
	static Registry& get()
	{
		static Registry registry;
		return registry;
	};

	entity add()
	{
		m_entities.push_back((IComponent**)malloc((uint32_t)IComponent::Type::COUNT * sizeof(IComponent*)));
		return m_entities.size();
	};

	void register_component(entity e, IComponent* cmp)
	{
		m_entities[e][(uint32_t)cmp->T] = cmp;
	};

	IComponent** get_components(entity e)
	{
		return m_entities[e];
	};

private:
	std::vector<IComponent**> m_entities;
};

template<typename T>
class ComponentSys
{
public:
	static ComponentSys<T>& get()
	{
		static ComponentSys<T> instance;
		return instance;
	};

	ComponentSys()
	{
		for (int i = 0; i < 100; i++)
			m_components[i] = nullptr;
	}

	T* add_component(entity e)
	{
		if (!m_components[e])
		{
			T* t = new T;
			m_components[e] = t;
			m_components_active.push_back(t);
			Registry::get().register_component(e, t);
			return t;
		}

		return nullptr;
	}

	void delete_component(entity e)
	{
		if (m_components[e])
		{
			m_components_active.erase(m_components[e]);
			delete m_components[e];
			m_components[e] = nullptr;
		}
	}

	T* get_component(entity e)
	{
		return m_components[e];
	}

	virtual void update()
	{
		for (auto cmp : m_components_active)
		{
			cmp->update();
		}
	}

protected:
	std::array<T*, 200> m_components;
	std::vector<T*> m_components_active;
	int m_size = 0;
};
