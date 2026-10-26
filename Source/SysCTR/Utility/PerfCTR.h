// Kirby 64 3DS: where each frame's time goes on the main core, for tuning toward 30fps.
// Times are summed between frames (svcGetSystemTick) and averaged per second; the bottom
// screen shows them under the FPS and perf.log in the data folder keeps a line every 5 s.
#pragma once

#include <3ds.h>

enum EPerfSlot
{
	PERF_DLIST,     // HLE graphics: display list processing (CPU side)
	PERF_GPU_WAIT,  // waiting in pglSwapBuffers for the previous frame's GPU work
	PERF_AUDIO,     // audio HLE on the main core (sync) or waiting for the async job
	PERF_MENU,      // drawing the bottom-screen menu
	PERF_IDLE,      // speed sync: sleeping because the frame finished early
	PERF_SLOTS
};

extern u64 gPerfTicks[PERF_SLOTS];

struct PerfScope
{
	EPerfSlot slot;
	u64       start;
	PerfScope(EPerfSlot s) : slot(s), start(svcGetSystemTick()) {}
	~PerfScope() { gPerfTicks[slot] += svcGetSystemTick() - start; }
};

// Called once per N64 frame (new VI origin); updates the averages shown by Perf_Text.
void        Perf_EndFrame();
const char* Perf_Text();
