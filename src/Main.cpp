#include "pch.h"
#include "Organism.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Engine/Physics.h"
#include "Engine/Time.h"
#include "Engine/App.h"

std::vector<Organism*> organisms;

static std::string get_random_DNA(unsigned max_length)
{
	auto vocabulary = "LTAM";
	auto params = "0123";
	std::stringstream dna;
	unsigned length = rand() % max_length + 1;
	for (unsigned i = 0; i < length; i++)
	{
		if (dna.str().find('M') == std::string::npos)
			dna << vocabulary[rand() % 4];
		else
			dna << vocabulary[rand() % 3];
		dna << "(";
		dna << rand() % 4;
		dna << ")";
	}

	return dna.str();
}


class AliveApp : public App
{
public:
	AliveApp() : App()
	{
		srand(time(NULL));

		Renderer2D::init();
		Renderer2D::setClearColor({ 0, 0, 0, 1 });

		TextureManager::add("cell_leaf.png");
		TextureManager::add("cell_thorn.png");
		TextureManager::add("cell_mover.png");
		TextureManager::add("cell_light.png");
		TextureManager::add("collider.png");
		TextureManager::add("light_circle.png");

		Physics::update(0.01f);
		Physics::update(0.01f);
		Physics::update(0.01f);
		Physics::update(0.01f);

		for (int i = 0; i < 300; i++)
		{
			auto* org = new Organism(get_random_DNA(5));
			org->set_transform(Random<Vec2>::range({ -m_window_data.Width * 0.5f, -m_window_data.Height * 0.5f }, { m_window_data.Width * 0.5f, m_window_data.Height * 0.5f }), Random<float>::range(0, 2 * std::numbers::pi));
			organisms.push_back(org);
		}
	}

	void update() override
	{
		auto projection = glm::ortho(-0.5f * m_window_data.Width, 0.5f * m_window_data.Width, -0.5f * m_window_data.Height, 0.5f * m_window_data.Height);

		for (auto org : organisms)
			org->tick();

		Physics::update(Time::Delta);

		Renderer2D::clear();
		Renderer2D::beginTextures(projection);

		for (auto org : organisms)
			org->draw();

		Renderer2D::endTextures();
	}
};

int main()
{
	AliveApp app;
	app.start();
	return 0;
}