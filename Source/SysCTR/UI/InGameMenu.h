#pragma once

#include "Core/RomSettings.h"

namespace UI
{
	void LoadRomPreferences(RomID mRomID);
	bool DrawOptionsPage(RomID mRomID);
	bool DrawInGameMenu(); // false when the bottom screen was left as it was
}