#include "LoaDebug.h"
#include "Util.h"
#include <HookManager.h>
#include <api/g2/ocnpc.h>
#include <api/g2/zcparser.h>
#include "api/g2/zcview.h"

//.text:006C86A0 public: virtual void __thiscall oCGame::Render(void) proc near
typedef void(__thiscall* OCGameRender)(void* pThis);
OCGameRender oCGameRender = (OCGameRender)0x006C86A0;

//.text:007A9A40 public: void __thiscall zCView::Print(int, int, class zSTRING const &) proc near
typedef void(__thiscall* ZCViewPrint)(void* pThis, int, int, zSTRING const &);
ZCViewPrint zCViewPrint = (ZCViewPrint)0x007A9A40;

//.text:00780D80 public: void __thiscall oCWorldTimer::Timer(void) proc near
typedef void(__thiscall* OCWorldTimerTimer)(void* pThis);
OCWorldTimerTimer oCWorldTimerTimer = (OCWorldTimerTimer)0x00780D80;

//.text:005F98A0 public: static void __cdecl zCTimer::FrameUpdate(void) proc near
typedef void(__cdecl* ZCTimerFrameUpdate)();
ZCTimerFrameUpdate zCTimerFrameUpdate = (ZCTimerFrameUpdate)0x005F98A0;


LoADebug::LoADebug()
{
	moduleDesc = "LoADebug";
}

LoADebug::~LoADebug()
{
}

void LoADebug::hookModule()
{
	HookManager* manager = HookManager::getHookManager();
	manager->addFunctionHook((LPVOID*)&zCTimerFrameUpdate, oCGameRenderHookNaked, moduleDesc);
}

void LoADebug::unHookModule()
{
	HookManager* manager = HookManager::getHookManager();
	manager->removeFunctionHook((LPVOID*)&zCTimerFrameUpdate, oCGameRenderHookNaked, moduleDesc);
}

void LoADebug::oCGameRenderHookNaked()
{
	_asm
	{
		LEGO_HOOKENGINE_PREAMBLE
		/*7 - 5 = 2 Byte for remaining opcode */
			nop
			nop
			/*finally hook function call*/
			jmp LoADebug::oCGameRenderHook
	}
}


void LoADebug::oCGameRenderHook()
{
	// Print hero.aivar[4] each frame
	oCNpc* hero = oCNpc::GetHero();
	if (hero)
	{
		static std::stringstream builder;
		builder << "Held ist sichtbar: ";
		bool unvisible = hero->aiscripts[4];
		if (unvisible)
		{
			builder << "Nein" << std::endl;
		} else
		{
			builder << "Ja" << std::endl;
		}

		zSTRING msg(builder.str().c_str());

		//clear stream for the next time!
		builder.str("");
		builder.clear();

		// call PrintScreen externals
		zCView* screen = zCView::GetScreen();
		screen->Print(200,200, msg);
	}

	zCTimerFrameUpdate();
}
