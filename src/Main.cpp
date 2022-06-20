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
#include "Mutation.h"

class AliveApp : public App
{
private:
	AliveApp() :
		dna_(dnaStr_)
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
		TextureManager::add("cell_mouth.png");
		TextureManager::add("cell_mouth_filled.png");
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
		OrganismManager::getInstance().draw();
		Renderer2D::endTextures();
		Renderer2D::setClearColor(BLACK);
	}

	void onKeyPressed(int key)
	{
		auto& io = ImGui::GetIO();
		if (io.WantCaptureKeyboard)
			return;

		OrthoCamController::getInstance().pressKey(key);

		if (key >= GLFW_KEY_1 && key < GLFW_KEY_1 + dnas_.size())
		{
			dnaIndex_ = (key - GLFW_KEY_1);
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
		auto& io = ImGui::GetIO();
		if (io.WantCaptureKeyboard)
			return;

		OrthoCamController::getInstance().releaseKey(key);

		if (key == GLFW_KEY_TAB)
		{
			m_is_tab_pressed = !m_is_tab_pressed;
		}
	}

	void onMouseScrolled(double offsetX, double offsetY)
	{
		auto& io = ImGui::GetIO();
		if (io.WantCaptureMouse)
			return;

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
			ImGuiWindowFlags windowFlags = 0;
			windowFlags  |= ImGuiWindowFlags_NoTitleBar;
			//windowFlags  |= ImGuiWindowFlags_MenuBar;
			//window_flags |= ImGuiWindowFlags_NoScrollbar;
			//windowFlags  |= ImGuiWindowFlags_NoMove;
			windowFlags  |= ImGuiWindowFlags_NoResize;
			windowFlags  |= ImGuiWindowFlags_NoCollapse;
			windowFlags  |= ImGuiWindowFlags_NoNav;
			windowFlags  |= ImGuiWindowFlags_NoBackground;
			//if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
			windowFlags |= ImGuiWindowFlags_NoDocking;
			//if (unsaved_document)   window_flags |= ImGuiWindowFlags_UnsavedDocument;
			bool* open = nullptr; // Don't pass our bool* to Begin

			ImGui::Begin("Map", open, windowFlags);
				ImGui::Text(std::format("Active Organisms: {}", Organism::Instances).c_str());
				ImGui::Text(std::format("OrganismManager::plants: {}", OrganismManager::getInstance().plants_.size()).c_str());
				ImGui::Text(std::format("OrganismManager::movers: {}", OrganismManager::getInstance().movers_.size()).c_str());
				ImGui::Text(std::format("OrganismManager::corpsesPlants: {}", OrganismManager::getInstance().corpsesPlants_.size()).c_str());
				ImGui::Text(std::format("OrganismManager::corpsesMovers: {}", OrganismManager::getInstance().corpsesMovers_.size()).c_str());
				ImGui::Text(std::format("Active Cells: {}", Cell::Instances).c_str());
				ImGui::InputFloat("Time Scale", &Time::Scale, 0.1f, 0.1f);
				ImGui::InputInt("Max Plants", &OrganismManager::MaxPlants, 100);
				ImGui::InputInt("One in N mutates", &Organism::OneInNMutates);
				ImGui::Checkbox("Allow Thorns", &DNA::AllowThorns);
				ImGui::Text(std::format("AABB count: {}", PhysicsManager::getInstance().getAABBCount()).c_str());
			ImGui::End();

			ImGui::Begin("DNA", open, windowFlags);
				ImGui::InputText("DNA", dnaStr_, 1000);
				dna_.setString(dnaStr_);

				if (ImGui::Button("Mutate"))
				{
					dna_.mutate();
					strcpy(dnaStr_, dna_.get().c_str());
				}
			ImGui::End();

			ImGui::Begin("Performance", open, windowFlags);

				for (const auto& profile : data)
					ImGui::Text(profile.c_str());

				float FPS = 1.0f / Time::DeltaSeconds;
				ImGui::Text(("FPS " + std::to_string(FPS)).c_str());
				ImGui::Text(("Simulation Time " + std::to_string(Time::ElapsedSeconds)).c_str());

			ImGui::End();
		}

		ScopeTimer::Data.clear();
	}

	void onMousePressed(int button) override
	{
		auto& io = ImGui::GetIO();
		if (io.WantCaptureMouse)
			return;

		if (button == GLFW_MOUSE_BUTTON_1)
		{
			auto mouse_pos_world = Camera::screenToWorldPoint(windowData_.mousePos, OrthoCamController::getInstance().getViewProjection());
			std::vector<Cell*> cells = Organism::getCellsForDNA(dna_.get());
			OrganismManager::getInstance().add(new Organism(dna_, cells, mouse_pos_world, Random::floatRange(0, 2 * std::numbers::pi)));
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
	char dnaStr_[1000] = "M(0)O(0)";
	DNA dna_;
	std::vector<std::string> dnas_ = {
		"L(0)",
		"M(0)O(0)",
		"M(0)O(0)T(0)", 
		"M(0)O(1)T(1)T(1)O(0)",
		"L(0)T(0)"
	};
	unsigned dnaIndex_ = 0;
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