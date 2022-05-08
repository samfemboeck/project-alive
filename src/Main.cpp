#include "pch.h"
#include "Organism.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Engine/Physics.h"
#include "Engine/Time.h"
#include "Engine/App.h"
#include "Engine/OrthoCamController.h"
#include "Engine/Timer.h"
#include "ContactListener.h"


static std::vector<Organism*> SOrganisms;
static long SInitialTTL = 5000;

static std::string get_random_DNA(unsigned max_length)
{
	auto vocabulary = "LTM";
	auto params = "0123";
	std::stringstream dna;
	unsigned length = rand() % max_length + 1;

	for (unsigned i = 0; i < length; i++)
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

static auto get_random_instinct()
{

}

class AliveApp : public App
{
public:
	AliveApp() :
		App()
	{
		srand(time(NULL));

		m_cam_controller.set_dimensions(m_window_data.Width, m_window_data.Height / (float)m_window_data.Width);

		Renderer2D::init();
		Renderer2D::setClearColor({ 0, 0, 0, 1 });

		TextureManager::add("cell_leaf.png");
		TextureManager::add("cell_thorn.png");
		TextureManager::add("cell_mover.png");
		TextureManager::add("cell_light.png");
		TextureManager::add("collider.png");
		TextureManager::add("light_circle.png");

		Physics::set_listener(new ContactListener());
	}

	void update() override
	{
		PROFILE("AliveApp::update()");
		m_cam_controller.update();

		{
			PROFILE("Organism::tick()");
			static std::vector<Organism*> to_delete;
			static std::vector<Organism*> to_add;

			for (auto* org : SOrganisms)
			{
				int result = org->tick();
				if (result != -1)
				{
					if (to_add.size() < Organism::MAX_INSTANCES)
					{
						for (unsigned i = 0; i < result; i++)
							to_add.push_back(org->clone());
					}

					to_delete.push_back(org);
				}
			}

			for (auto* org : to_delete)
			{
				delete org;
				std::erase(SOrganisms, org);
			}

			for (auto* org : to_add)
				SOrganisms.push_back(org);

			to_delete.clear();
			to_add.clear();
		}

		{
			PROFILE("Physics::update()");
			Physics::update(Time::DeltaSeconds);
		}

		{
			Renderer2D::clear();
			Renderer2D::beginTextures(m_cam_controller.get_view_projection());

			{
				{
					PROFILE("Organism::draw_light()");
					for (auto* org : SOrganisms)
						org->draw_light();
				}
				{
					PROFILE("Organism::draw()");
					for (auto* org : SOrganisms)
						org->draw();
				}
			}

			{
				PROFILE("Renderer2D::endTextures()");
				Renderer2D::endTextures();
			}
		}
	}

	void key_pressed(int key) override
	{
		m_cam_controller.key_pressed(key);

		if (key >= GLFW_KEY_1 && key < GLFW_KEY_1 + Organism::DEFAULT_DNAS.size())
		{
			Organism::DNA_INDEX = (key - GLFW_KEY_1);
			LOG("Selected DNA: {}", Organism::DEFAULT_DNAS[Organism::DNA_INDEX]);
		}

		static const float scroll_speed_ttl = 250;

		if (key == GLFW_KEY_UP)
		{
			Organism::TTL += scroll_speed_ttl;
			LOG("TTL: {}", Organism::TTL);
		}
		else if (key == GLFW_KEY_DOWN)
		{
			Organism::TTL -= scroll_speed_ttl;
			LOG("TTL: {}", Organism::TTL);
		}
	}

	void key_released(int key) override
	{
		m_cam_controller.key_released(key);
	}

	void mouse_scrolled(double x_offset, double y_offset) override
	{
		m_cam_controller.mouse_scrolled(y_offset);
	}

	virtual void mouse_pressed(int button) 
	{
		if (button == 0)
		{
			float amplitude = Random<float>::range(0.25f, 3.0f);
			float offset_sin1 = Random<float>::range(0.0f, 10.0f);
			float multiplier_x_sin1 = Random<int>::range(1, 5);
			float offset_sin2 = Random<float>::range(0.0f, 10.0f);
			float multiplier_x_sin2 = Random<int>::range(1, 5);

			auto instinct = [=](float x) -> float
			{
				return amplitude * (sin(multiplier_x_sin1 * x + offset_sin1) + sin(multiplier_x_sin2 * x + offset_sin2));
			};

			auto mouse_pos_world = Camera::screen_to_world_point({ m_window_data.MousePos.X / m_window_data.Width, m_window_data.MousePos.Y / m_window_data.Height }, m_cam_controller.get_view_projection());
			SOrganisms.push_back(new Organism(Organism::DEFAULT_DNAS[Organism::DNA_INDEX], instinct, mouse_pos_world, Random<float>::range(0, 2 * std::numbers::pi)));
		}
	};

private:
	OrthoCamController m_cam_controller;

};

int main()
{
	AliveApp app;
	app.start();
	return 0;
}