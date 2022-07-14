#pragma once

#include "Engine/App.h"
#include "pch.h"
#include "Engine/FrameBuffer.h"
#include "Mutation.h"

struct Slot
{
	enum Rarity
	{
		Common,
		Rare
	};

	bool isLocked = true;
	DNA DNA;
	Texture2D* texture;
	Rarity rarity;
};

class AliveApp : public App
{
private:
	AliveApp();
public:
	void onShutdown() override;
	void startSound();
	static AliveApp& getInstance();
	void onUpdate();
	void onKeyPressed(int key);
	void onKeyReleased(int key);
	void onMouseScrolled(double offsetX, double offsetY);
	void onDrawImGui();
	void onMousePressed(int button) override;
	void onMouseReleased(int button) override;
	void embraceTheDarkness();
	void initDockspace();

private:
	bool isRunning_ = true;
	bool showDecomposerModal_ = false;
	bool showUnlockNewOrgModal_ = false;
	bool showNewDominatingSpeciesModal_ = false;
	bool showPredatorsModal_ = false;
	bool showParasitesModal_ = false;
	bool showThornsModal_ = false;
	bool showWikiUpdatedModal_ = false;
	bool showFiveCellDominationModal_ = false;
	bool showPredatorDominationModal_ = false;
	bool showFPSModal_ = false;
	bool showHelpModal_ = true;
	std::array<Slot, 8> dnaSlots_;
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
	unsigned activeSoundIdx_ = 0;
	bool soundActive_ = false;
};
