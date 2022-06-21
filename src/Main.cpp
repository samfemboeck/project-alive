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

		TextureManager::add("cell_leaf_filled.png");
		TextureManager::add("cell_thorn_filled.png");
		TextureManager::add("cell_mover_filled.png");
		TextureManager::add("cell_light_filled.png");
		TextureManager::add("cell_fruit_filled.png");
		TextureManager::add("cell_food.png");
		TextureManager::add("cell_food_filled.png");
		TextureManager::add("cell_mouth_filled.png");
		TextureManager::add("aabb.png");
		TextureManager::add("organism_worm_1.png");
		TextureManager::add("organism_worm_2.png");
		TextureManager::add("organism_predator_1.png");
		TextureManager::add("organism_predator_2.png");
		TextureManager::add("organism_plant_1.png");
		TextureManager::add("organism_plant_2.png");

		dnaSlots_[0] = DNA("MO");
		dnaSlots_[1] = DNA("MT");
		dnaSlots_[2] = DNA("P");
		dnaSlots_[3] = DNA("MMO");
		dnaSlots_[4] = DNA("MTRTRM");
		dnaSlots_[5] = DNA("P[P][RP][RRP][LP]");
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
		Renderer2D::beginTextures(OrthoCamController::getInstance().getView(), OrthoCamController::getInstance().getProjection());
		OrganismManager::getInstance().draw();
		Renderer2D::endTextures();
		frameBuffer_.unbind();
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
		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

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

		bool showDemo = false;
		ImGui::ShowDemoWindow(&showDemo);

		ImGuiWindowFlags windowFlags = 0;
		windowFlags |= ImGuiWindowFlags_NoTitleBar;
		//windowFlags  |= ImGuiWindowFlags_MenuBar;
		window_flags |= ImGuiWindowFlags_NoScrollbar;
		windowFlags |= ImGuiWindowFlags_NoMove;
		windowFlags |= ImGuiWindowFlags_NoResize;
		windowFlags |= ImGuiWindowFlags_NoCollapse;
		windowFlags |= ImGuiWindowFlags_NoNav;
		windowFlags |= ImGuiWindowFlags_NoBackground;
		//if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
		//windowFlags |= ImGuiWindowFlags_NoDocking;
		//if (unsaved_document)   window_flags |= ImGuiWindowFlags_UnsavedDocument;
		bool* open = nullptr; // Don't pass our bool* to Begin

		ImGui::Begin("Settings", open);
		ImGui::SliderFloat("Sim Speed", &Time::Scale, 0.5f, 20.0f, "%.0f");
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Speed of simulation time.");

		ImGui::InputInt("Mutation Rate", &Organism::OneInNMutates);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("One in x Organisms will mutate.");

		ImGui::End();

		{
			ImGui::Begin("World", open);

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

		{
			ImGui::Begin("Organisms");
			auto imgWidth = ImGui::GetContentRegionAvail().x / 3.0f;

			Texture2D* texture1 = TextureManager::get("organism_worm_1.png");
			Texture2D* texture2 = TextureManager::get("organism_predator_1.png");
			Texture2D* texture3 = TextureManager::get("organism_plant_1.png");
			Texture2D* texture4 = TextureManager::get("organism_worm_2.png");
			Texture2D* texture5 = TextureManager::get("organism_predator_2.png");
			Texture2D* texture6 = TextureManager::get("organism_plant_2.png");

			float aspect = texture1->getAspect();
			int framePadding = 0;
			ImVec4 bgColorSelected = { 0, 0, 1, 0.1f };
			ImVec4 bgColorDefault = { 0, 0, 0, 0 };

			if (ImGui::ImageButton((void*)texture1->getId(), { imgWidth, imgWidth / aspect }, { 0, 0 }, { 1, 1 }, framePadding, slotIdx_ == 0 ? bgColorSelected : bgColorDefault))
				slotIdx_ = 0;

			ImGui::SameLine();

			if (ImGui::ImageButton((void*)texture2->getId(), { imgWidth, imgWidth / aspect }, { 0, 0 }, { 1, 1 }, framePadding, slotIdx_ == 1 ? bgColorSelected : bgColorDefault))
				slotIdx_ = 1;

			ImGui::SameLine();

			if (ImGui::ImageButton((void*)texture3->getId(), { imgWidth, imgWidth / aspect }, { 0, 0 }, { 1, 1 }, framePadding, slotIdx_ == 2 ? bgColorSelected : bgColorDefault))
				slotIdx_ = 2;

			if (ImGui::ImageButton((void*)texture4->getId(), { imgWidth, imgWidth / aspect }, { 0, 0 }, { 1, 1 }, framePadding, slotIdx_ == 3 ? bgColorSelected : bgColorDefault))
				slotIdx_ = 3;

			ImGui::SameLine();

			if (ImGui::ImageButton((void*)texture5->getId(), { imgWidth, imgWidth / aspect }, { 0, 0 }, { 1, 1 }, framePadding, slotIdx_ == 4 ? bgColorSelected : bgColorDefault))
				slotIdx_ = 4;

			ImGui::SameLine();

			if (ImGui::ImageButton((void*)texture6->getId(), { imgWidth, imgWidth / aspect }, { 0, 0 }, { 1, 1 }, framePadding, slotIdx_ == 5 ? bgColorSelected : bgColorDefault))
				slotIdx_ = 5;

			ImGui::End();
		}
	
		if (false)
		{
			ImGui::Begin("Performance", open);

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
		if (io.WantCaptureMouse && !isHoveringSimWindow_)
			return;

		if (button == GLFW_MOUSE_BUTTON_1 && isHoveringSimWindow_)
		{
			auto mouse_pos_world = Camera::screenToWorldPoint(mousePos_, OrthoCamController::getInstance().getViewProjection());
			std::vector<Cell*> cells = Organism::getCellsForDNA(dnaSlots_[slotIdx_].get());
			OrganismManager::getInstance().add(new Organism(dnaSlots_[slotIdx_], cells, mouse_pos_world, Random::floatRange(0, 2 * std::numbers::pi)));
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
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

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

private:
	std::array<DNA, 6> dnaSlots_;
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