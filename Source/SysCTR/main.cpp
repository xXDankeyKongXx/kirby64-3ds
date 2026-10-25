#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>

#include <3ds.h>
#include <GL/picaGL.h>

#include "BuildOptions.h"
#include "Config/ConfigOptions.h"
#include "Core/Cheats.h"
#include "Core/CPU.h"
#include "Core/CPU.h"
#include "Core/Memory.h"
#include "Core/PIF.h"
#include "Core/RomSettings.h"
#include "Core/Save.h"
#include "Debug/DBGConsole.h"
#include "Debug/DebugLog.h"
#include "Graphics/GraphicsContext.h"
#include "HLEGraphics/TextureCache.h"
#include "Input/InputManager.h"
#include "Interface/RomDB.h"
#include "System/Paths.h"
#include "System/System.h"
#include "Test/BatchTest.h"

#include "UI/UserInterface.h"
#include "UI/RomSelector.h"

#include "Utility/IO.h"
#include "Utility/Preferences.h"
#include "Utility/Profiler.h"
#include "Utility/Thread.h"
#include "Utility/Translate.h"
#include "Utility/Timer.h"
#include "Utility/ROMFile.h"
#include "Utility/MemoryCTR.h"

bool isN3DS = false;
bool shouldQuit = false;

EAudioPluginMode enable_audio = APM_ENABLED_ASYNC;

#ifdef DAEDALUS_LOG
void log2file(const char *format, ...) {
	__gnuc_va_list arg;
	int done;
	va_start(arg, format);
	char msg[512];
	done = vsprintf(msg, format, arg);
	va_end(arg);
	sprintf(msg, "%s\n", msg);
	FILE *log = fopen("sdmc:/DaedalusX64.log", "a+");
	if (log != NULL) {
		fwrite(msg, 1, strlen(msg), log);
		fclose(log);
	}
}
#endif

static void CheckDSPFirmware()
{
	FILE *firmware = fopen("sdmc:/3ds/dspfirm.cdc", "rb");

	if(firmware != NULL)
	{
		fclose(firmware);
		return;
	}

	gfxInitDefault();
	consoleInit(GFX_BOTTOM, NULL);

	printf("DSP Firmware not found!\n\n");
	printf("Press START to exit\n");

	while(aptMainLoop())
	{
		hidScanInput();

		if(hidKeysDown() == KEY_START)
			exit(1);
	}
}

// Kirby 64 3DS: copy a bundled data file from RomFS to the SD folder on first run
static void SeedDataFile(const char *name)
{
	char dst[256], src[256];
	sprintf(dst, "%s%s", DAEDALUS_CTR_PATH(""), name);
	sprintf(src, "romfs:/%s", name);

	FILE *out = fopen(dst, "rb");
	if(out != NULL) { fclose(out); return; }

	FILE *in = fopen(src, "rb");
	if(in == NULL) return;
	out = fopen(dst, "wb");
	if(out != NULL)
	{
		char buf[4096];
		size_t n;
		while((n = fread(buf, 1, sizeof(buf), in)) > 0) fwrite(buf, 1, n, out);
		fclose(out);
	}
	fclose(in);
}

// Kirby 64 3DS: names accepted for the ROM inside sdmc:/3ds/Kirby64/
static const char * const kRomNames[] = {
	"kirby64.z64", "kirby64.n64", "kirby64.v64",
	"Kirby_64_-_The_Crystal_Shards__USA_.z64",
	"Kirby 64 - The Crystal Shards (USA).z64",
	NULL
};

static bool FindKirbyRom(char *out)
{
	for(int i = 0; kRomNames[i] != NULL; i++)
	{
		sprintf(out, "%s%s", DAEDALUS_CTR_PATH(""), kRomNames[i]);
		FILE *f = fopen(out, "rb");
		if(f != NULL) { fclose(f); return true; }
	}
	return false;
}

static void ShowMissingRom()
{
	consoleInit(GFX_BOTTOM, NULL);
	printf("Kirby 64 ROM not found!\n\n");
	printf("Copy your own ROM dump to:\n");
	printf("  sdmc:/3ds/Kirby64/kirby64.z64\n\n");
	printf("Press START to exit\n");

	while(aptMainLoop())
	{
		hidScanInput();
		if(hidKeysDown() & KEY_START) break;
		gfxFlushBuffers();
		gfxSwapBuffers();
		gspWaitForVBlank();
	}
}

static void Initialize()
{
	CheckDSPFirmware();
	
	_InitializeSvcHack();

	romfsInit();
	
	APT_CheckNew3DS(&isN3DS);
	osSetSpeedupEnable(true);
	
	gfxInit(GSP_BGR8_OES, GSP_BGR8_OES, true);

	if(isN3DS)
		gfxSetWide(true);
	
	pglInitEx(0x080000, 0x040000);

	IO::Directory::EnsureExists( DAEDALUS_CTR_PATH("") );
	IO::Directory::EnsureExists( DAEDALUS_CTR_PATH("SaveGames/") );
	SeedDataFile("roms.ini");
	SeedDataFile("Daedalus.cht");

	strcpy(gDaedalusExePath, DAEDALUS_CTR_PATH(""));
	strcpy(g_DaedalusConfig.mSaveDir, DAEDALUS_CTR_PATH("SaveGames/"));

	IO::Directory::EnsureExists( DAEDALUS_CTR_PATH("SaveStates/") );
	UI::Initialize();

	System_Init();
}


void HandleEndOfFrame()
{
	shouldQuit = !aptMainLoop();
	
	if (shouldQuit)
	{
		CPU_Halt("Exiting");
	}
}

extern u32 __ctru_heap_size;

int main(int argc, char* argv[])
{
	char fullpath[512];

	Initialize();

	// Kirby 64 3DS: no ROM menu, boot straight into the game
	if(!FindKirbyRom(fullpath))
	{
		ShowMissingRom();
	}
	else if(System_Open(fullpath))
	{
		CPU_Run();
		System_Close();
	}

	System_Finalize();
	pglExit();

	return 0;
}
