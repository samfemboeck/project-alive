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
#include "OrganismManager.h"

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

class AliveApp : public App
{
private:
	AliveApp()	
	{
		srand(time(NULL));

		OrthoCamController::getInstance().setDimensions(windowData_.width, windowData_.height / (float)windowData_.width);

		Renderer2D::init();
		Renderer2D::setClearColor({ 0, 0, 0, 1 });

		TextureManager::add("cell_leaf.png");
		TextureManager::add("cell_leaf_filled.png");
		TextureManager::add("cell_thorn.png");
		TextureManager::add("cell_thorn_filled.png");
		TextureManager::add("cell_mover.png");
		TextureManager::add("cell_mover_filled.png");
		TextureManager::add("cell_light.png");
		TextureManager::add("cell_light_filled.png");
		TextureManager::add("cell_fruit.png");
		TextureManager::add("cell_fruit_filled.png");
		TextureManager::add("cell_food.png");
		TextureManager::add("cell_food_filled.png");
		TextureManager::add("collider.png");
		TextureManager::add("light_circle.png");
		TextureManager::add("light_warm.png");
		TextureManager::add("background.png");
		TextureManager::add("aabb.png");
	}

public:
	static AliveApp& getInstance()
	{
		static AliveApp app;
		return app;
	}

	void onUpdate()
	{
		if (isRightMouseDown_)
		{
			auto offset = Camera::screenToWorldPoint(windowData_.mousePos, OrthoCamController::getInstance().getViewProjection()) - Camera::screenToWorldPoint(mousePosDown_, OrthoCamController::getInstance().getViewProjection());
			OrthoCamController::getInstance().setPosition(camPos_ - offset);
		}

		OrganismManager::getInstance().update();

		{
			ScopeTimer timer("Tick Physics");
			PhysicsManager::getInstance().update();
		}

		Renderer2D::clear();
		Renderer2D::beginTextures(OrthoCamController::getInstance().getView(), OrthoCamController::getInstance().getProjection());
		PhysicsManager::getInstance().draw();
		OrganismManager::getInstance().draw();
		Renderer2D::endTextures();
		Renderer2D::setClearColor(BLACK);
	}

	void onKeyPressed(int key)
	{
		OrthoCamController::getInstance().pressKey(key);

		if (key >= GLFW_KEY_1 && key < GLFW_KEY_1 + Organism::DefaultDNAs.size())
		{
			Organism::DNAIndex = (key - GLFW_KEY_1);
		}

		static const float scroll_speed_ttl = 250;

		if (key == GLFW_KEY_UP)
		{
			Organism::MaxTTL += scroll_speed_ttl;
		}
		else if (key == GLFW_KEY_DOWN)
		{
			Organism::MaxTTL -= scroll_speed_ttl;
		}
	}

	void onKeyReleased(int key)
	{
		OrthoCamController::getInstance().releaseKey(key);

		if (key == GLFW_KEY_TAB)
		{
			m_is_tab_pressed = !m_is_tab_pressed;
		}
	}

	void onMouseScrolled(double offsetX, double offsetY)
	{
		OrthoCamController::getInstance().scrollMouse(offsetY);
		//
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
			ImGui::InputInt("Max Instances", &OrganismManager::MaxInstances, 100);
			ImGui::Text(std::format("Selected DNA: {}", Organism::DefaultDNAs[Organism::DNAIndex]).c_str());
			ImGui::Text(std::format("Time To Live: {}", Organism::MaxTTL).c_str());
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
		}

		ScopeTimer::Data.clear();
	}

	void onMousePressed(int button) override
	{
		if (button == GLFW_MOUSE_BUTTON_1)
		{
			float amplitude = Random::floatRange(1.0f, 10.0f);
			float offset_sin1 = Random::floatRange(0.0f, 10.0f);
			float multiplier_x_sin1 = Random::floatRange(1, 5);
			float offset_sin2 = Random::floatRange(0.0f, 10.0f);
			float multiplier_x_sin2 = Random::floatRange(1, 5);

			auto instinct = [=](float x) -> float
			{
				return amplitude * (sin(multiplier_x_sin1 * x + offset_sin1) + sin(multiplier_x_sin2 * x + offset_sin2));
			};

			auto mouse_pos_world = Camera::screenToWorldPoint(windowData_.mousePos, OrthoCamController::getInstance().getViewProjection());
			OrganismManager::getInstance().add(new Organism(Organism::DefaultDNAs[Organism::DNAIndex], instinct, mouse_pos_world, Random::floatRange(0, 2 * std::numbers::pi)));
		}
		else if (button == GLFW_MOUSE_BUTTON_2)
		{
			mousePosDown_ = windowData_.mousePos;
			isRightMouseDown_ = true;
			camPos_ = OrthoCamController::getInstance().getPosition();
		}
	}

	void onMouseReleased(int button) override
	{
		if (button == GLFW_MOUSE_BUTTON_2)
			isRightMouseDown_ = false;
	}

private:
	bool m_is_tab_pressed = false;
	Vec2f mousePosDown_;
	Vec2f camPos_;
	bool isRightMouseDown_ = false;
};

int main()
{
	try
	{
		auto& app = AliveApp::getInstance();
		app.start();
	}
	catch (std::exception e)
	{
		LOG("{}", e.what());
	}
	return 0;
}