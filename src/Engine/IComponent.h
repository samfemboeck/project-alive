#pragma once

struct IComponent
{
};

template<typename T, typename S>
class ComponentSys
{
public:
	static S& get()
	{
		static S s;
		return s;
	};

	ComponentSys()
	{
		for (int i = 0; i < 100; i++)
			m_components[i] = nullptr;
	}

	void add_component(entity e)
	{
		if (!m_components[e])
		{
			T* t = new T;
			m_components[e] = t;
			m_components_active.push_back(t);
		}
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

protected:
	std::array<T*, 200> m_components;
	std::vector<T*> m_components_active;
	int m_size = 0;
};
