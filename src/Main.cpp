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
#include "Engine/SoundManager.h"
#include "Main.h"

/*
	--- TODOs ---
	- Achievements
	- Wiki	
	- Performance
		- Multithreaded Physik
		- Redirections loswerden
		- Auto simulation scale adjustment
	- Fix Physics

	--- Achievements ---
	- 50 plant corpses -> Herbivore "Worm" (Trivial)
	- New Species is dominating species -> Herbivore "Glider" (Trivial)
	- 5 Predators of same DNA -> Predator "Worm" (Common)
	- 20 Parasites in World -> "Parasite" (Common)
	- 80% of plants have thorns (Common)
	- Dominating Species has 5 Cells -> Herbivore "Figure 9" (Rare)
	- Predators are over 50% of mover population -> Predator "Figure 9" (Rare)
*/

AliveApp::AliveApp() :
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
	TextureManager::add("organism_glider.png");
	TextureManager::add("organism_herbivore_figure9.png");
	TextureManager::add("organism_parasite.png");
	TextureManager::add("organism_plant_2.png");
	TextureManager::add("organism_plant_invincible.png");
	TextureManager::add("organism_predator_centipede.png");
	TextureManager::add("organism_predator_2.png");
	TextureManager::add("organism_worm_1.png");
	TextureManager::add("organism_predator_worm.png");
	TextureManager::add("questionmark.png");

	SoundManager::init();
	SoundManager::add("drop_0.wav");
	SoundManager::add("drop_1.wav");
	SoundManager::add("drop_2.wav");
	SoundManager::add("drop_3.wav");
	SoundManager::add("drop_4.wav");
	SoundManager::add("1.wav");
	SoundManager::add("2.wav");
	SoundManager::add("3.wav");
	SoundManager::add("4.wav");
	SoundManager::add("5.wav");
	SoundManager::add("6.wav");
	SoundManager::add("7.wav");
	SoundManager::add("8.wav");
	SoundManager::add("9.wav");
	SoundManager::add("10.wav");
	SoundManager::add("11.wav");
	SoundManager::add("12.wav");

	bool cheat = false;

	Slot slot1;
	slot1.DNA = DNA("P[P]");
	slot1.isLocked = false;
	slot1.texture = TextureManager::get("organism_plant_2.png");
	slot1.rarity = Slot::Rarity::Common;
	dnaSlots_[0] = slot1;

	Slot slot2;
	slot2.DNA = DNA("MO");
	slot2.isLocked = !cheat;
	slot2.texture = TextureManager::get("organism_worm_1.png");
	slot2.rarity = Slot::Rarity::Common;
	dnaSlots_[1] = slot2;

	Slot slot3;
	slot3.DNA = DNA("MO[RM]");
	slot3.isLocked = !cheat;
	slot3.texture = TextureManager::get("organism_glider.png");
	slot3.rarity = Slot::Rarity::Common;
	dnaSlots_[2] = slot3;

	Slot slot4;
	slot4.DNA = DNA("MMC");
	slot4.isLocked = !cheat;
	slot4.texture = TextureManager::get("organism_predator_worm.png");
	slot4.rarity = Slot::Rarity::Common;
	dnaSlots_[3] = slot4;

	Slot slot5;
	slot5.DNA = DNA("O");
	slot5.isLocked = !cheat;
	slot5.texture = TextureManager::get("organism_parasite.png");
	slot5.rarity = Slot::Rarity::Common;
	dnaSlots_[4] = slot5;

	Slot slot6;
	slot6.DNA = DNA("P[T][RT][RRT][LT]");
	slot6.isLocked = !cheat;
	slot6.texture = TextureManager::get("organism_plant_invincible.png");
	slot6.rarity = Slot::Rarity::Rare;
	dnaSlots_[5] = slot6;

	Slot slot7;
	slot7.DNA = DNA("MMM[LC][RC]C");
	slot7.isLocked = !cheat;
	slot7.texture = TextureManager::get("organism_predator_centipede.png");
	slot7.rarity = Slot::Rarity::Rare;
	dnaSlots_[6] = slot7;

	Slot slot8;
	slot8.DNA = DNA("MMOLOLM");
	slot8.isLocked = !cheat;
	slot8.texture = TextureManager::get("organism_herbivore_figure9.png");
	slot8.rarity = Slot::Rarity::Rare;
	dnaSlots_[7] = slot8;

	GLFWimage images[1];
	images[0].pixels = stbi_load("Textures/organism_plant_invincible.png", &images[0].width, &images[0].height, 0, 4); //rgba channels 
	glfwSetWindowIcon(window_, 1, images);
	stbi_image_free(images[0].pixels);
}

void AliveApp::onShutdown()
{
}

void AliveApp::startSound()
{
	soundActive_ = true;
}

AliveApp& AliveApp::getInstance()
{
	static AliveApp app;
	return app;
}

void AliveApp::onUpdate()
{	
	frameBuffer_.bind();
	Renderer2D::clear();
	Renderer2D::setClearColor({ 0.0f, 0.0f, 0.0f, 0.0f });
	OrganismManager::getInstance().draw();
	EntityGrid::getInstance().draw();
	frameBuffer_.unbind();

	if (isRightMouseDown_)
	{
		auto offset = Camera::screenToWorldPoint(mousePos_, OrthoCamController::getInstance().getViewProjection()) - Camera::screenToWorldPoint(mousePosDown_, OrthoCamController::getInstance().getViewProjection());
		OrthoCamController::getInstance().setPosition(camPos_ - offset);
	}

	if (!isRunning_)
		return;

	OrganismManager::getInstance().update();

	{
		ScopeTimer timer("Tick Physics");
		PhysicsManager::getInstance().update();
	}


	static bool triggeredEventDecomposers = false;
	if (!triggeredEventDecomposers && OrganismManager::getInstance().getNumPlantCorpses() > 800)
	{
		triggeredEventDecomposers = true;
		showDecomposerModal_ = true;
		dnaSlots_[1].isLocked = false;
	}

	static bool triggeredEventNewDominatingSpecies = false;
	if (!triggeredEventNewDominatingSpecies && OrganismManager::getInstance().getDominatingSpecies() != "MO")
	{
		auto dominatingSpecies = OrganismManager::getInstance().getDominatingSpecies();
		std::string::difference_type numOs = std::count(dominatingSpecies.begin(), dominatingSpecies.end(), 'O');
		std::string::difference_type numMs = std::count(dominatingSpecies.begin(), dominatingSpecies.end(), 'M');

		if (numOs + numMs > 2)
		{
			triggeredEventNewDominatingSpecies = true;
			showNewDominatingSpeciesModal_ = true;
			dnaSlots_[2].isLocked = false;
		}
	}

	static bool triggeredEventPredators = false;
	if (!triggeredEventPredators && OrganismManager::getInstance().getPredatorEventTriggered())
	{
		triggeredEventPredators = true;
		showPredatorsModal_ = true;
		dnaSlots_[3].isLocked = false;
	}

	static bool triggeredEventParasites = false;
	if (!triggeredEventParasites && OrganismManager::getInstance().getParasiteEventTriggered())
	{
		triggeredEventParasites = true;
		showParasitesModal_ = true;
		dnaSlots_[4].isLocked = false;
	}

	static bool triggeredEventThorns = false;
	if (!triggeredEventThorns && OrganismManager::getInstance().getThornEventTriggered())
	{
		triggeredEventThorns = true;
		showThornsModal_ = true;
		dnaSlots_[5].isLocked = false;
	}

	static bool triggeredEventPredatorDomination = false;
	if (!triggeredEventPredatorDomination && OrganismManager::getInstance().getPredatorDominationEventTriggered())
	{
		triggeredEventPredatorDomination = true;
		showPredatorDominationModal_ = true;
		dnaSlots_[6].isLocked = false;
	}

	static bool triggeredFiveCellDominationEvent = false;
	if (!triggeredFiveCellDominationEvent && OrganismManager::getInstance().getFiveCellDominationEventTriggered())
	{
		triggeredFiveCellDominationEvent = true;
		showFiveCellDominationModal_ = true;
		dnaSlots_[7].isLocked = false;
	}

	static bool triggeredThornCellDiscoveredEvent = false;
	if (!triggeredThornCellDiscoveredEvent && OrganismManager::getInstance().getThornCellDiscovered())
	{
		triggeredThornCellDiscoveredEvent = true;
		showWikiUpdatedModal_ = true;
	}

	static bool triggeredHerbivoreDiscoveredEvent = false;
	if (!triggeredHerbivoreDiscoveredEvent && OrganismManager::getInstance().getHerbivoreDiscovered())
	{
		triggeredHerbivoreDiscoveredEvent = true;
		showWikiUpdatedModal_ = true;
	}

	static bool triggeredCarnivoreDiscoveredEvent = false;
	if (!triggeredCarnivoreDiscoveredEvent && OrganismManager::getInstance().getCarnivoreDiscovered())
	{
		triggeredCarnivoreDiscoveredEvent = true;
		showWikiUpdatedModal_ = true;
	}

	if (soundActive_)
	{
		std::string activeFilename = std::to_string(activeSoundIdx_ + 1) + ".wav";

		if (!SoundManager::isPlaying(activeFilename))
		{
			SoundManager::start(activeFilename);
		}
		else if (SoundManager::isEnd(activeFilename))
		{
			activeSoundIdx_++;

			if (activeSoundIdx_ == 12)
				activeSoundIdx_ = 0;

			SoundManager::reset(activeFilename);
		}
	}
}

void AliveApp::onKeyPressed(int key)
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

void AliveApp::onKeyReleased(int key)
{
	auto& io = ImGui::GetIO();
	if (io.WantCaptureKeyboard)
		return;

	OrthoCamController::getInstance().releaseKey(key);
}

void AliveApp::onMouseScrolled(double offsetX, double offsetY)
{
	auto& io = ImGui::GetIO();
	if (io.WantCaptureMouse && !isHoveringSimWindow_)
		return;

	if (isHoveringSimWindow_)
		OrthoCamController::getInstance().scrollMouse(offsetY);
}

void AliveApp::onDrawImGui()
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
		if (isRunning_)
		{
			if (ImGui::Button("Pause"))
			{
				isRunning_ = false;
			}
		}
		else
		{
			if (ImGui::Button("Resume"))
			{
				isRunning_ = true;
			}
		}

		if (ImGui::Button("Wiki"))
		{
			showHelpModal_ = true;
		}

		ImGui::EndMainMenuBar();
	}

	{
		ImGui::Begin("Simulation Speed", open, windowFlags);

		ImGui::PushItemWidth(-1);
		ImGui::SliderFloat("##Speed", &Time::Scale, 0.5f, 8.0f, "%.1f");
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Speed of simulation time.");
		ImGui::PopItemWidth();

		ImGui::End();
	}

	{
		ImGui::Begin("Organisms", open, windowFlags);
		unsigned orgsPerCell = 2;
		auto& style = ImGui::GetStyle();
		float imgWidth = (ImGui::GetContentRegionAvail().x - ((orgsPerCell - 1) * style.FramePadding.x)) / (float)orgsPerCell;

		float aspect = dnaSlots_[0].texture->getAspect();
		ImVec4 bgColorSelected = { 0, 0, 1, 0.1f };
		ImVec4 bgColorDefault = { 0, 0, 0, 0 };

		Texture2D* textureLock = TextureManager::get("questionmark.png");

		static bool allSlotsUnlocked = false;
		unsigned unlockedCount = 0;
		for (unsigned i = 0, count = 0; i < dnaSlots_.size(); i++)
		{
			if (dnaSlots_[i].isLocked)
			{
				ImGui::ImageButton((void*)textureLock->getId(), { imgWidth, imgWidth / aspect }, { 0, 1 }, { 1, 0 }, 0, bgColorDefault);

				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::Text("Rarity: ");
					ImGui::SameLine();
					ImVec4 color;
					std::string txt;

					switch (dnaSlots_[i].rarity)
					{
					case Slot::Rarity::Common:
						color = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
						txt = "Common";
						break;
					case Slot::Rarity::Rare:
						color = ImVec4(1.0f, 0.0f, 1.0f, 1.0f);
						txt = "Rare";
						break;
					}

					ImGui::TextColored(color, txt.c_str());
					ImGui::Text("Discover a special event to unlock.");
					ImGui::EndTooltip();
				}
			}
			else
			{
				if (!allSlotsUnlocked)
				{
					unlockedCount++;
					if (unlockedCount = dnaSlots_.size())
					{
						allSlotsUnlocked = true;
						ImGui::OpenPopup("You discovered every event!");
					}
				}

				if (ImGui::ImageButton((void*)dnaSlots_[i].texture->getId(), { imgWidth, imgWidth / aspect }, { 0, 1 }, { 1, 0 }, 0, slotIdx_ == i ? bgColorSelected : bgColorDefault))
					slotIdx_ = i;

				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("Left click inside the white rectangle to spawn.");
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

	{
		ImGui::Begin("FPS", open, windowFlags);

		static unsigned frameCount = 0;
		float FPS = 1.0f / Time::DeltaSeconds;

		if (FPS >= 10)
		{
			frameCount = 0;
		}
		else
		{
			frameCount++;
			if (frameCount > 20)
			{
				frameCount = 0;
				showFPSModal_ = true;
				Time::Scale -= 1;
			}
		}

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

	/*
	{
		ImGui::Begin("Registry");

		const auto& registry = OrganismManager::getInstance().getRegistry();
		for (const auto& pair : registry)
		{
			if (pair.second > 0)
			{
				std::string text = pair.first + ": " + std::to_string(pair.second);
				ImGui::Text(text.c_str());
			}
		}

		ImGui::End();
	}
	*/

	{
		if (showUnlockNewOrgModal_)
		{
			showUnlockNewOrgModal_ = false;
			auto& io = ImGui::GetIO();
			ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
			ImGui::OpenPopup("New organism unlocked!");
		}

		if (showDecomposerModal_)
		{
			showDecomposerModal_ = false;
			auto& io = ImGui::GetIO();
			ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
			ImGui::OpenPopup("Decomposers");
			isRunning_ = false;
		}

		if (showNewDominatingSpeciesModal_)
		{
			showNewDominatingSpeciesModal_ = false;
			auto& io = ImGui::GetIO();
			ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
			ImGui::OpenPopup("Survival of the fittest");
			isRunning_ = false;
		}

		if (showPredatorsModal_)
		{
			showPredatorsModal_ = false;
			auto& io = ImGui::GetIO();
			ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
			ImGui::OpenPopup("Predator and prey");
			isRunning_ = false;
		}

		if (showParasitesModal_)
		{
			showParasitesModal_ = false;
			auto& io = ImGui::GetIO();
			ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
			ImGui::OpenPopup("Symbiosis");
			isRunning_ = false;
		}

		if (showWikiUpdatedModal_)
		{
			showWikiUpdatedModal_ = false;
			auto& io = ImGui::GetIO();
			ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
			ImGui::OpenPopup("The wiki has been updated!");
			isRunning_ = false;
		}

		if (showThornsModal_)
		{
			showThornsModal_ = false;
			auto& io = ImGui::GetIO();
			ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
			ImGui::OpenPopup("Evolutionary arms race");
			isRunning_ = false;
		}

		if (showFiveCellDominationModal_)
		{
			showFiveCellDominationModal_ = false;
			auto& io = ImGui::GetIO();
			ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
			ImGui::OpenPopup("Size matters");
			isRunning_ = false;
		}

		if (showPredatorDominationModal_)
		{
			showPredatorDominationModal_ = false;
			auto& io = ImGui::GetIO();
			ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
			ImGui::OpenPopup("Supply and demand");
			isRunning_ = false;
		}

		if (showFPSModal_)
		{
			showFPSModal_ = false;
			auto& io = ImGui::GetIO();
			ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
			ImGui::OpenPopup("Performance issues");
		}

		if (showHelpModal_)
		{
			showHelpModal_ = false;
			ImGui::OpenPopup("Wiki");
		}

		if (ImGui::BeginPopupModal("Decomposers"))
		{
			ImGui::Text("Plants are about to go extinct!");
			ImGui::TextWrapped(
				"Decomposers play a critical role in the flow of energy through an ecosystem. They break apart dead organisms into simpler inorganic materials, "
				"making nutrients available to primary producers - mostly plants and algae."
			);

			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
				showUnlockNewOrgModal_ = true;
			}

			ImGui::SetItemDefaultFocus();
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupModal("Survival of the fittest"))
		{
			ImGui::Text("A new species is dominating!");
			ImGui::TextWrapped(
				"Whether or not a species will prevail in nature depends on its fitness regarding the environment. The creature capable of "
				"creating the most copies of itself has an edge in surviving. Eventually, less adapted creatures will die out in the process."
			);

			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
				showUnlockNewOrgModal_ = true;
			}
		}

		if (ImGui::BeginPopupModal("Predator and prey"))
		{
			ImGui::Text("A species of carnivores is emerging!");
			ImGui::TextWrapped(
				"Carnivores are animals that prey on other animals. Hunters kill living animals for food. Scavengers feast on corpses in order to survive. More often than not, carnivore species are both hunters and scavengers."
				"Meat contains more energy than plant matter. That's why carnivores don't have to eat as much as herbivores do."
			);

			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
				showUnlockNewOrgModal_ = true;
			}
		}

		if (ImGui::BeginPopupModal("Symbiosis"))
		{
			ImGui::Text("Organisms with a single pink cell are spreading! They are getting pushed around by other creatures.");
			ImGui::TextWrapped(
				"Different species often inhabit the same spaces and share - or compete for - the same resources. They interact in a variety of ways, known collectively as symbiosis."
				"Different forms of symbiotic relationships exist. This particular form is called commensalism. One species benefits from the other while not harming it."
			);

			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
				showUnlockNewOrgModal_ = true;
			}
		}

		if (ImGui::BeginPopupModal("The wiki has been updated!"))
		{
			ImGui::TextWrapped("Access the wiki by pressing the 'Wiki' button at the top left of the application. The simulation has been paused. Resume by pressing the 'Resume' button at the top left of the application.");

			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
			}
		}

		if (ImGui::BeginPopupModal("Evolutionary arms race"))
		{
			ImGui::Text("Half of all plants developed thorns!");
			ImGui::TextWrapped(
				"Species, in order to evolve, must accumulate adaptations that are favorable for the environment in which they live."
				"These preferred traits are what makes an individual more fit and able to live long enough to reproduce."
				"Plants develop thorns so that they are less likely to be consumed by roaming herbivores."
			);

			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
				showUnlockNewOrgModal_ = true;
			}
		}

		if (ImGui::BeginPopupModal("Size matters"))
		{
			ImGui::Text("More than half of all moving herbivore creatures have 5 cells or more!");
			ImGui::TextWrapped(
				"Sometimes nature favors the survival of big animals. A big body allows them to defend against smaller predators and the environment (e.g. thorns)."
			);

			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
				showUnlockNewOrgModal_ = true;
			}
		}

		if (ImGui::BeginPopupModal("Supply and demand"))
		{
			ImGui::Text("Carnivores constitute more than one third of the moving population!");
			ImGui::TextWrapped(
				"Carnivorism is a biological niche which thrives if enough prey is around."
			);

			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
				showUnlockNewOrgModal_ = true;
			}
		}

		if (ImGui::BeginPopupModal("New organism unlocked!"))
		{
			ImGui::TextWrapped("Check your 'Organisms' window for new species. The simulation has been paused. Resume by pressing the 'Resume' button at the top left of the application.");

			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::SetItemDefaultFocus();
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupModal("Performance issues"))
		{
			ImGui::TextWrapped("The simulation speed was reduced due to bad performance.");

			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupModal("You discovered every event!"))
		{
			ImGui::Text("Congratulations! Thanks for playing.");
			ImGui::EndPopup();
		}

		bool unusedOpen = true;
		if (ImGui::BeginPopupModal("Wiki", &unusedOpen))
		{
			if (ImGui::BeginTabBar("MyTabBar"))
			{
				if (ImGui::BeginTabItem("About"))
				{	
					ImGui::TextWrapped(
						"This is the wiki window. It has multiple tabs and can be reopened by clicking on the 'Wiki' button on the top left of the application. " 
						"Please note: This wiki will be updated with new tabs as the simulation progresses! Please come back here after you get notified."
					);

					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Organisms"))
				{
					ImGui::TextWrapped(
						"Every entity in this simulation is an organism. Organisms are made up of one or more cells of a specific type and color. "
						"They can reproduce once a specific criteria is met and enough space is available around them. "
						"After a certain amount of time, every organism will die and every cell will be replaced with a gray corpse cell."
					);
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Green Cell"))
				{
					ImGui::TextWrapped(
						"Represents plants. Organisms with green cells will reproduce at a set interval. No further requirements are neccessary."
					);
					ImGui::EndTabItem();
				}

				if (OrganismManager::getInstance().getThornCellDiscovered())
				{
					if (ImGui::BeginTabItem("Dark Red Cell"))
					{
						ImGui::TextWrapped("Represents thorns. Organisms with green cells may grow this cell when mutating. When an organism touches a dark red cell, it will die. Unless it has more cells than the organism owning this cell.");
						ImGui::EndTabItem();
					}
				}

				if (OrganismManager::getInstance().getHerbivoreDiscovered())
				{
					if (ImGui::BeginTabItem("Yellow Cell"))
					{
						ImGui::TextWrapped("Represents legs.");
						ImGui::EndTabItem();
					}

					if (ImGui::BeginTabItem("Pink Cell"))
					{
						ImGui::TextWrapped("Represents the mouth of a herbivore.");
						ImGui::EndTabItem();
					}
				}

				if (OrganismManager::getInstance().getCarnivoreDiscovered())
				{
					if (ImGui::BeginTabItem("Light Red Cell"))
					{
						ImGui::TextWrapped("Represents the mouth of a carnivore.");
					}
				}

				ImGui::EndTabBar();
			}

		
			ImGui::EndPopup();
		}
	}

	ScopeTimer::Data.clear();
}

void AliveApp::onMousePressed(int button)
{
	auto& io = ImGui::GetIO();
	if (io.WantCaptureMouse && !isHoveringSimWindow_)
		return;

	if (button == GLFW_MOUSE_BUTTON_1 && isHoveringSimWindow_)
	{
		auto mousePosWorld = Camera::screenToWorldPoint(mousePos_, OrthoCamController::getInstance().getViewProjection());
		std::vector<Cell*> cells = Organism::getCellsForDNA(dnaSlots_[slotIdx_].DNA.get());
		OrganismManager::getInstance().add(new Organism(dnaSlots_[slotIdx_].DNA, cells, mousePosWorld, Random::floatRange(0, 2 * std::numbers::pi)));
		unsigned soundIdx = Random::unsignedRange(0, 4);
		std::string filename = "drop_" + std::to_string(soundIdx) + ".wav";
		SoundManager::oneShot(filename);
	}
	else if (button == GLFW_MOUSE_BUTTON_2 && isHoveringSimWindow_)
	{
		mousePosDown_ = mousePos_;
		isRightMouseDown_ = true;
		camPos_ = OrthoCamController::getInstance().getPosition();
	}
}

void AliveApp::onMouseReleased(int button)
{
	if (button == GLFW_MOUSE_BUTTON_2)
		isRightMouseDown_ = false;
}

void AliveApp::embraceTheDarkness()
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
	colors[ImGuiCol_TitleBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
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
	style.WindowPadding = ImVec2(0.00f, 0.00f);
	style.FramePadding = ImVec2(5.00f, 5.00f);
	style.CellPadding = ImVec2(6.00f, 6.00f);
	style.ItemSpacing = ImVec2(6.00f, 6.00f);
	style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
	style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
	style.IndentSpacing = 25;
	style.ScrollbarSize = 15;
	style.GrabMinSize = 40;
	style.WindowBorderSize = 0;
	style.ChildBorderSize = 0;
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
	style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
}

void AliveApp::initDockspace()
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