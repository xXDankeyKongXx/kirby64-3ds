#include <stdio.h>

#include "PerfCTR.h"
#include "Utility/IO.h"

u64 gPerfTicks[PERF_SLOTS];

static u64  window_start;
static u32  window_frames;
static u32  windows;
static char text[96] = "";

void Perf_EndFrame()
{
	u64 now = svcGetSystemTick();
	if(window_start == 0) { window_start = now; return; }
	window_frames++;

	const u64 elapsed = now - window_start;
	if(elapsed < SYSCLOCK_ARM11) return; // average over a second

	const float to_ms = 1000.0f / SYSCLOCK_ARM11 / window_frames;
	const float frame = elapsed * to_ms;
	float slot[PERF_SLOTS], other = frame;
	for(int i = 0; i < PERF_SLOTS; i++)
	{
		slot[i] = gPerfTicks[i] * to_ms;
		gPerfTicks[i] = 0;
	}
	// the menu is drawn from inside display list processing (BeginFrame)
	slot[PERF_DLIST] = slot[PERF_DLIST] > slot[PERF_MENU] ? slot[PERF_DLIST] - slot[PERF_MENU] : 0.0f;
	for(int i = 0; i < PERF_SLOTS; i++)
		other -= slot[i];

	// "cpu" is everything else: the dynarec running the game, interrupts, input
	snprintf(text, sizeof(text), "%.1fms  cpu %.1f  gfx %.1f  gpu %.1f  snd %.1f  ui %.1f  idle %.1f",
		frame, other, slot[PERF_DLIST], slot[PERF_GPU_WAIT], slot[PERF_AUDIO], slot[PERF_MENU], slot[PERF_IDLE]);

	if(++windows % 5 == 0)
	{
		FILE* f = fopen(DAEDALUS_CTR_PATH("perf.log"), windows == 5 ? "w" : "a");
		if(f) { fprintf(f, "%.1f fps %s\n", window_frames * (float)SYSCLOCK_ARM11 / elapsed, text); fclose(f); }
	}

	window_start = now;
	window_frames = 0;
}

const char* Perf_Text()
{
	return text;
}
