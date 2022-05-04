#include <stdlib.h>
#include <glm/glm.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <vector>
#include <string>
#include <time.h>
#include <sstream>
#include <glm/ext/matrix_clip_space.hpp>

import App;
import Organism;
import Renderer;
import Physics;
import Timer;

static std::string randomDNA()
{
	auto vocabulary = "LTM";
	auto params = "0123";
	std::stringstream dna;
	int length = rand() % 6;
	for (int i = 0; i < length; i++)
	{
		if (dna.str().find('M') == std::string::npos)
			dna << vocabulary[rand() % 3];
		else
			dna << vocabulary[rand() % 2];
		dna << "(";
		dna << rand() % 4;
		dna << ")";
	}
	
	return dna.str();
}

class AliveApp : public App
{
public:
	AliveApp()
	{
		Renderer::get().init();
		Renderer::get().setClearColor(glm::vec4{ 0, 0, 0, 1 });

		srand(time(NULL));

		TextureManager::get().add("cell_leaf.png");
		TextureManager::get().add("cell_thorn.png");
		TextureManager::get().add("cell_mover.png");
		TextureManager::get().add("collider.png");

		for (int i = 0; i < 200; i++)
		{
			auto org = new Organism(randomDNA());
			organisms.push_back(org);
			org->m_target_point = Random::vec2_range({ -m_window_width * 0.5f, -m_window_height * 0.5f }, { m_window_width * 0.5f, m_window_height * 0.5f });
			org->set_position(Random::vec2_range({ -m_window_width * 0.5f, -m_window_height * 0.5f }, { m_window_width * 0.5f, m_window_height * 0.5f }));
		}
	}

	void update(float deltaTime) override
	{
		static glm::mat4 projection = glm::ortho(-0.5f * m_window_width, 0.5f * m_window_width, -0.5f * m_window_height, 0.5f * m_window_height);

		for (auto org : organisms)
			org->tick(deltaTime);

		Physics::update(deltaTime);

		Renderer::get().clear();
		Renderer::get().beginTextures(projection);

		for (auto org : organisms)
			org->draw();

		Renderer::get().endTextures();
	}

private:
	std::vector<Organism*> organisms;
};

int main()
{
	AliveApp app;
	app.start();
	return 0;
}