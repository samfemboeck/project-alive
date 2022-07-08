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
#include "Engine/FrameBuffer.h"

/*
	--- TODOs ---
	- "Add 1 Random Cell" als Mutation
	- Multithreaded Physik
	- Simulation Tutorial
	- Achievements

	--- Achievements ---
	- Game started and all corpses -> Herbivore "Worm"
	- New Species is dominating species -> Herbivore "Glider"
	- 5 Predators of same DNA -> Predator "Worm"
	- Dominating Species has 5 Cells -> Herbivore "Figure 9"
	- Dominating Species has 10 Cells -> "Invincible Plant" 
	- Predators are over 50% of mover population -> Predator "Figure 9"
	- 20 Parasites in World -> "Parasite"
*/

struct Slot
{
	bool isLocked = true;
	DNA DNA;
	Texture2D* texture;
};

class AliveApp : public App
{
private:
	AliveApp() :
		dna_(dnaStr_),
		frameBuffer_(1920.0f, 1080.0f)
	{
		srand(time(NULL));

		OrthoCamController::getInstance().setDimensions(windowData_.width, windowData_.height / (float)windowData_.width);

		Renderer2D::init();
		Renderer2D::setClearColor({ 0, 0, 0, 1 });

		TextureManager::add("cell_plant.png");
		TextureManager::add("cell_thorn.png");
		TextureManager::add("cell_carnivore.png");
		TextureManager::add("cell_herbivore.png");
		TextureManager::add("cell_mover.png");
		TextureManager::add("cell_corpse.png");
		TextureManager::add("cell_armor.png");
		TextureManager::add("aabb.png");
		TextureManager::add("organism_worm_1.png");
		TextureManager::add("organism_worm_2.png");
		TextureManager::add("organism_predator_1.png");
		TextureManager::add("organism_predator_2.png");
		TextureManager::add("organism_plant_1.png");
		TextureManager::add("organism_plant_2.png");
		TextureManager::add("organism_parasite.png");
		TextureManager::add("lock.png");

		Slot slot1;
		slot1.DNA = DNA("P[P]");
		slot1.isLocked = false;
		slot1.texture = TextureManager::get("organism_plant_2.png");
		dnaSlots_[0] = slot1;

		Slot slot2;
		slot2.DNA = DNA("MO");
		slot2.isLocked = false;
		slot2.texture = TextureManager::get("organism_worm_1.png");
		dnaSlots_[1] = slot2;

		Slot slot3;
		slot3.DNA = DNA("MT");
		slot3.isLocked = true;
		slot3.texture = TextureManager::get("organism_predator_1.png");
		dnaSlots_[2] = slot3;

		Slot slot4;
		slot4.DNA = DNA("O");
		slot4.isLocked = true;
		slot4.texture = TextureManager::get("organism_parasite.png");
		dnaSlots_[3] = slot4;

		GLFWimage images[1]; 
		images[0].pixels = stbi_load("Textures/appicon.png", &images[0].width, &images[0].height, 0, 4); //rgba channels 
		glfwSetWindowIcon(window_, 1, images);
		stbi_image_free(images[0].pixels);
	}

public:
	static AliveApp& getInstance()
	{
		static AliveApp app;
		return app;
	}

	void onUpdate()
	{
		if (!isRunning_)
			return;

		if (isRightMouseDown_)
		{
			auto offset = Camera::screenToWorldPoint(mousePos_, OrthoCamController::getInstance().getViewProjection()) - Camera::screenToWorldPoint(mousePosDown_, OrthoCamController::getInstance().getViewProjection());
			OrthoCamController::getInstance().setPosition(camPos_ - offset);
		}

		OrganismManager::getInstance().update();

		{
			ScopeTimer timer("Tick Physics");
			PhysicsManager::getInstance().update();
		}

		frameBuffer_.bind();
		Renderer2D::clear();
		Renderer2D::setClearColor({ 0.0f, 0.0f, 0.0f, 0.0f });
		OrganismManager::getInstance().draw();
		EntityGrid::getInstance().draw();
		frameBuffer_.unbind();

		if (!isDiscoveredDecomposers_ && !wantsToDiscoverDecomposers_ && OrganismManager::getInstance().plants_.size() > 0)
			wantsToDiscoverDecomposers_ = true;

		if (wantsToDiscoverDecomposers_ && OrganismManager::getInstance().corpsesPlants_.size() > 10)
		{
			isDiscoveredDecomposers_ = true;
			wantsToDiscoverDecomposers_ = false;
			dnaSlots_[1].isLocked = false;
		}
	}

	void onKeyPressed(int key)
	{
		auto& io = ImGui::GetIO();
		if (io.WantCaptureKeyboard)
			return;

		OrthoCamController::getInstance().pressKey(key);

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
	}

	void onMouseScrolled(double offsetX, double offsetY)
	{
		auto& io = ImGui::GetIO();
		if (io.WantCaptureMouse && !isHoveringSimWindow_)
			return;

		if (isHoveringSimWindow_)
			OrthoCamController::getInstance().scrollMouse(offsetY);
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

		embraceTheDarkness();
		initDockspace();

		bool showDemo = false;
		//ImGui::ShowDemoWindow(&showDemo);

		ImGuiWindowFlags windowFlags = 0;
		//windowFlags |= ImGuiWindowFlags_NoTitleBar;
		//windowFlags  |= ImGuiWindowFlags_MenuBar;
		windowFlags |= ImGuiWindowFlags_NoScrollbar;
		windowFlags |= ImGuiWindowFlags_NoMove;
		windowFlags |= ImGuiWindowFlags_NoResize;
		windowFlags |= ImGuiWindowFlags_NoCollapse;
		windowFlags |= ImGuiWindowFlags_NoNav;
		windowFlags |= ImGuiWindowFlags_NoBackground;
		windowFlags |= ImGuiWindowFlags_NoScrollWithMouse;
		//if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
		//windowFlags |= ImGuiWindowFlags_NoDocking;
		//if (unsaved_document)   window_flags |= ImGuiWindowFlags_UnsavedDocument;
		bool* open = nullptr; // Don't pass our bool* to Begin

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::Button("Pause"))
			{
			}
			if (ImGui::Button("Quit"))
			{
				glfwSetWindowShouldClose(window_, true);
			}
			ImGui::EndMainMenuBar();
		}

		ImGui::Begin("Simulation Speed", open, windowFlags);

		ImGui::PushItemWidth(-1);
		ImGui::SliderFloat("##Speed", &Time::Scale, 0.5f, 8.0f, "%.1f");
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Speed of simulation time.");
		ImGui::PopItemWidth();

		ImGui::End();

	
		{
			ImGui::Begin("Organisms", open, windowFlags);
			unsigned orgsPerCell = 2;
			auto& style = ImGui::GetStyle();
			float imgWidth = (ImGui::GetContentRegionAvail().x - ((orgsPerCell - 1) * style.FramePadding.x)) / (float)orgsPerCell;			

			float aspect = dnaSlots_[0].texture->getAspect();
			ImVec4 bgColorSelected = { 0, 0, 1, 0.1f };
			ImVec4 bgColorDefault = { 0, 0, 0, 0 };

			Texture2D* textureLock = TextureManager::get("lock.png");

			for (unsigned i = 0, count = 0; i < dnaSlots_.size(); i++)
			{
				if (dnaSlots_[i].isLocked)
				{
					ImGui::ImageButton((void*)textureLock->getId(), {imgWidth, imgWidth / aspect}, {0, 1}, {1, 0}, 0, bgColorDefault);
				}
				else
				{
					if (ImGui::ImageButton((void*)dnaSlots_[i].texture->getId(), { imgWidth, imgWidth / aspect }, { 0, 1 }, { 1, 0 }, 0, slotIdx_ == i ? bgColorSelected : bgColorDefault))
						slotIdx_ = i;
				}

				if (count == orgsPerCell - 1)
				{
					count = 0;
				}
				else
				{
					ImGui::SameLine();
					count++;
				}
			}

			ImGui::End();
		}

		if (true)
		{	
			ImGuiWindowFlags windowFlags = 0;
			//windowFlags |= ImGuiWindowFlags_NoTitleBar;
			//windowFlags  |= ImGuiWindowFlags_MenuBar;
			windowFlags |= ImGuiWindowFlags_NoScrollbar;
			windowFlags |= ImGuiWindowFlags_NoMove;
			windowFlags |= ImGuiWindowFlags_NoResize;
			windowFlags |= ImGuiWindowFlags_NoCollapse;
			windowFlags |= ImGuiWindowFlags_NoNav;
			windowFlags |= ImGuiWindowFlags_NoBackground;
			windowFlags |= ImGuiWindowFlags_NoScrollWithMouse;

			ImGui::Begin("FPS", open, windowFlags);

			float FPS = 1.0f / Time::DeltaSeconds;
			ImGui::PushItemWidth(-1);
			ImGui::Text((std::to_string((int)FPS)).c_str());
			ImGui::PopItemWidth();
			ImGui::End();
		}

		{
			ImGuiWindowClass windowClass;
			windowClass.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_AutoHideTabBar;
			ImGui::SetNextWindowClass(&windowClass);
			ImGui::Begin("World", open, windowFlags);

			bool isWindowFocused = ImGui::IsWindowFocused();
			isHoveringSimWindow_ = ImGui::IsWindowHovered();

			Vec2f viewportOffset = { ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y };
			Vec2f mousePosImGui = Vec2f(ImGui::GetMousePos().x, ImGui::GetMousePos().y);
			Vec2f mousePos = mousePosImGui - viewportOffset;
			mousePos_ = mousePos / size_;

			float width = ImGui::GetContentRegionAvail().x;
			float aspect = 9.0f / 16.0f;
			float height = aspect * width;

			if (size_.x != width || size_.y != height)
			{
				frameBuffer_.setSize(width, height);
				size_ = { width, height };
			}

			ImGui::Image((void*)frameBuffer_.getColorBufferRendererId(), *(ImVec2*)&size_, { 0, 1 }, { 1, 0 });
			ImGui::End();
		}

		static bool wasPopupDisplayed = false;
		if (isDiscoveredDecomposers_ && !wasPopupDisplayed)
		{
			ImGui::OpenPopup("Decomposers");
			wasPopupDisplayed = true;
			isRunning_ = false;
		}

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal("Decomposers", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Decomposers are necessary to recycle nutrients and let new plants grow!");
			ImGui::Separator();

			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
				isRunning_ = true;
			}

			ImGui::SetItemDefaultFocus();
			ImGui::EndPopup();
		}

		ScopeTimer::Data.clear();
	}

	void onMousePressed(int button) override
	{
		auto& io = ImGui::GetIO();
		if (io.WantCaptureMouse && !isHoveringSimWindow_)
			return;

		if (button == GLFW_MOUSE_BUTTON_1 && isHoveringSimWindow_)
		{
			if (isRunning_)
			{
				auto mousePosWorld = Camera::screenToWorldPoint(mousePos_, OrthoCamController::getInstance().getViewProjection());
				std::vector<Cell*> cells = Organism::getCellsForDNA(dnaSlots_[slotIdx_].DNA.get());
				OrganismManager::getInstance().add(new Organism(dnaSlots_[slotIdx_].DNA, cells, mousePosWorld, Random::floatRange(0, 2 * std::numbers::pi)));
			}
			else
			{
				auto mousePosWorld = Camera::screenToWorldPoint(mousePos_, OrthoCamController::getInstance().getViewProjection());
				EntityGrid& grid = EntityGrid::getInstance();
				auto localPos = grid.getLocalCoord(mousePosWorld);
				auto& entitiesInSquare = grid.get(localPos.x, localPos.y);

				for (auto entity : entitiesInSquare)
				{
					if (entity && entity->bounds.contains(mousePosWorld))
					{
						LOG("DNA: {}", entity->organism->getDNA().str());
					}
				}
			}
		}
		else if (button == GLFW_MOUSE_BUTTON_2 && isHoveringSimWindow_)
		{
			mousePosDown_ = mousePos_;
			isRightMouseDown_ = true;
			camPos_ = OrthoCamController::getInstance().getPosition();
		}
	}

	void onMouseReleased(int button) override
	{
		if (button == GLFW_MOUSE_BUTTON_2)
			isRightMouseDown_ = false;
	}


	void embraceTheDarkness()
	{
		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
		colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
		colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
		colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
		colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_DockingPreview] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.5);

		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowPadding = ImVec2(8.00f, 8.00f);
		style.FramePadding = ImVec2(5.00f, 2.00f);
		style.CellPadding = ImVec2(6.00f, 6.00f);
		style.ItemSpacing = ImVec2(6.00f, 6.00f);
		style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
		style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
		style.IndentSpacing = 25;
		style.ScrollbarSize = 15;
		style.GrabMinSize = 10;
		style.WindowBorderSize = 1;
		style.ChildBorderSize = 1;
		style.PopupBorderSize = 1;
		style.FrameBorderSize = 1;
		style.TabBorderSize = 1;
		style.WindowRounding = 7;
		style.ChildRounding = 4;
		style.FrameRounding = 3;
		style.PopupRounding = 4;
		style.ScrollbarRounding = 9;
		style.GrabRounding = 3;
		style.LogSliderDeadzone = 4;
		style.TabRounding = 4;
	}

	void initDockspace()
	{
		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None; //ImGuiDockNodeFlags_NoResize;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 200.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		style.WindowMinSize.x = minWinSizeX;
		ImGui::End();
	}

private:
	bool isRunning_ = true;
	bool isDiscoveredDecomposers_ = false;
	bool wantsToDiscoverDecomposers_ = false;
	std::array<Slot, 4> dnaSlots_;
	unsigned slotIdx_ = 0;
	Vec2f size_ = { 16.0f, 9.0f };
	Vec2f mousePos_;
	bool isHoveringSimWindow_ = false;
	FrameBuffer frameBuffer_;
	Vec2f mousePosDown_;
	Vec2f camPos_;
	bool isRightMouseDown_ = false;
	char dnaStr_[1000] = "M(0)O(0)";
	DNA dna_;
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