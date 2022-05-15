#include "pch.h"
#include "Organism.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Engine/Physics.h"
#include "Engine/Time.h"
#include "Engine/App.h"
#include "Engine/OrthoCamController.h"
#include "Engine/Timer.h"
#include <thread>
#include "Engine/Renderer2D.h"
#include "Cell.h"

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
std::array<std::array<wchar_t, 5>, 5000> test_arr;

class AliveApp : public App
{
public:
	AliveApp() :
		App()
	{
		srand(time(NULL));

		m_cam_controller.setDimensions(windowData_.width, windowData_.height / (float)windowData_.width);

		Renderer2D::init();
		Renderer2D::setClearColor({ 0, 0, 0, 1 });

		TextureManager::add("cell_leaf.png");
		TextureManager::add("cell_thorn.png");
		TextureManager::add("cell_mover.png");
		TextureManager::add("cell_light.png");
		TextureManager::add("collider.png");
		TextureManager::add("light_circle.png");
		TextureManager::add("light_warm.png");
		TextureManager::add("background.png");
	}

	void onUpdate()
	{
		ScopeTimer timer("Update Main");
		m_cam_controller.update();

		{
			ScopeTimer timer("Tick Organisms");
			static std::vector<Organism*> to_delete;
			static std::vector<int> results;

			for (auto* org : SOrganisms)
			{
				int result = org->tick();

				if (result != -1)
				{
					results.push_back(result);
					to_delete.push_back(org);
				}
			}			
			
			for (auto* org : to_delete)
			{
				std::erase(SOrganisms, org);
			}

			for (unsigned i = 0; i < results.size(); i++)
			{
				if (SOrganisms.size() < Organism::MaxInstances)
				{
					for (unsigned a = 0; a < results[i]; a++)
						SOrganisms.push_back(to_delete[i]->clone());
				}

				delete to_delete[i];
			}			

			to_delete.clear();
			results.clear();
		}

		{
			ScopeTimer timer("Tick Physics");
			PhysicsManager::getInstance().update();
			//Physics::update(Time::DeltaSeconds);
		}

		{
			Renderer2D::clear();
			Renderer2D::beginTextures(m_cam_controller.getView(), m_cam_controller.getProjection());
			//Renderer2D::pushQuad(glm::scale(glm::mat4(1.0f), { m_window_data.Width * 4, m_window_data.Height * 4, 1 }), TextureManager::get("background.png"), glm::vec4(1.0f), false);
			{
				{
					ScopeTimer timer("Push Cells");
					for (auto* org : SOrganisms)
						org->draw();
				}
			}
			{
				ScopeTimer timer("Flush");
				Renderer2D::endTextures();
				Renderer2D::setClearColor(BLACK);
			}
		}
	}

	void onKeyPressed(int key)
	{
		m_cam_controller.pressKey(key);

		if (key >= GLFW_KEY_1 && key < GLFW_KEY_1 + Organism::DefaultDNAs.size())
		{
			Organism::DNAIndex = (key - GLFW_KEY_1);
		}

		static const float scroll_speed_ttl = 250;

		if (key == GLFW_KEY_UP)
		{
			Organism::TimeToLive += scroll_speed_ttl;
		}
		else if (key == GLFW_KEY_DOWN)
		{
			Organism::TimeToLive -= scroll_speed_ttl;
		}
	}

	void onKeyReleased(int key)
	{
		m_cam_controller.releaseKey(key);		

		if (key == GLFW_KEY_TAB)
		{
			m_is_tab_pressed = !m_is_tab_pressed;
		}
	}

	void onMouseScrolled(double offsetX, double offsetY)
	{
		m_cam_controller.scrollMouse(offsetY);
	}

	void onDrawImGui()
	{
		static std::vector<std::string> data;
		static float elapsed = 0;
		elapsed += Time::DeltaSeconds;

		if (elapsed > 1)
		{
			elapsed = 0;
			data = ScopeTimer::Data;
		}

		if (m_is_tab_pressed)
		{
			bool show_demo = false;
			//ImGui::ShowDemoWindow(&show_demo);
			ImGui::Begin("Organism");
			ImGui::InputInt("Max Instances", &Organism::MaxInstances, 100);
			ImGui::Text(std::format("Selected DNA: {}", Organism::DefaultDNAs[Organism::DNAIndex]).c_str());
			ImGui::Text(std::format("Time To Live: {}", Organism::TimeToLive).c_str());
			ImGui::Text(std::format("Active Organisms: {}", Organism::ActiveInstances).c_str());
			ImGui::Text(std::format("Active Cells: {}", Cell::Instances).c_str());
			ImGui::End();
			ImGui::Begin("Physics");
			ImGui::InputFloat("Linear Friction", &RigidBody::LinearFriction);
			ImGui::End();
			ImGui::Begin("Performance");

			for (const auto& profile : data)
				ImGui::Text(profile.c_str());

			ImGui::End();
			ImGui::Begin("Water");
			ImGui::ColorEdit3("Water Color", &Renderer2DStorage::WaterColor.x);
			ImGui::InputInt("Tile Level", &Renderer2DStorage::TileLevel);
			ImGui::InputFloat3("Light Attenuation", &Renderer2DStorage::LightAttenuation.x);
			ImGui::InputFloat3("Ambient Light", &Renderer2DStorage::AmbientLight.x);
			ImGui::End();
			//
		}

		ScopeTimer::Data.clear();
	}

	void onMousePressed(int button)
	{
		if (button == 0)
		{
			float amplitude = Random<float>::range(3.0f, 30.0f);
			float offset_sin1 = Random<float>::range(0.0f, 10.0f);
			float multiplier_x_sin1 = Random<int>::range(1, 5);
			float offset_sin2 = Random<float>::range(0.0f, 10.0f);
			float multiplier_x_sin2 = Random<int>::range(1, 5);

			auto instinct = [=](float x) -> float
			{
				return amplitude * (sin(multiplier_x_sin1 * x + offset_sin1) + sin(multiplier_x_sin2 * x + offset_sin2));
			};

			auto mouse_pos_world = Camera::screenToWorldPoint({ windowData_.mousePos.x / windowData_.width, windowData_.mousePos.y / windowData_.height }, m_cam_controller.getViewProjection());
			SOrganisms.push_back(new Organism(Organism::DefaultDNAs[Organism::DNAIndex], instinct, mouse_pos_world, Random<float>::range(0, 2 * std::numbers::pi)));
		}
	}

private:
	OrthoCamController m_cam_controller;
	bool m_is_tab_pressed = false;

};

int main()
{
	AliveApp app;
	app.start();
	return 0;
}