/*////////////////////////////////////////////////////////////////////////////

This file is part of the G2Ext SDK headers.

//////////////////////////////////////////////////////////////////////////////

The G2Ext SDK headers

Copyright © 2009, 2010 by Paindevs and Patrick Vogel

All Rights reserved.

THE WORK (AS DEFINED BELOW) IS PROVIDED
UNDER THE TERMS OF THIS CREATIVE COMMONS
PUBLIC LICENSE ("CCPL" OR "LICENSE").
THE WORK IS PROTECTED BY COPYRIGHT AND/OR
OTHER APPLICABLE LAW. ANY USE OF THE WORK
OTHER THAN AS AUTHORIZED UNDER THIS LICENSE
OR COPYRIGHT LAW IS PROHIBITED.

BY EXERCISING ANY RIGHTS TO THE WORK PROVIDED
HERE, YOU ACCEPT AND AGREE TO BE BOUND BY THE
TERMS OF THIS LICENSE. TO THE EXTENT THIS
LICENSE MAY BE CONSIDERED TO BE A CONTRACT,
THE LICENSOR GRANTS YOU THE RIGHTS CONTAINED
HERE IN CONSIDERATION OF YOUR ACCEPTANCE OF
SUCH TERMS AND CONDITIONS.

Full license at http://creativecommons.org/licenses/by-nc/3.0/legalcode

/////////////////////////////////////////////////////////////////////////////*/

#ifndef __ZTIMER_H_INCLUDED__
#define __ZTIMER_H_INCLUDED__

#ifndef __G2EXT_API_HEADER
#define __G2EXT_API_HEADER
#endif  //__G2EXT_API_HEADER

#ifdef _G2EXT_COMPILE_SPACER
#error Cannot use gothic headers on spacer dll (_G2EXT_COMPILE_SPACER defined)
#endif

#include "api/g2/ztypes.h"
#include "api/g2/macros.h"

/** Insert description. */
class zCTimer
{
public:

	zREAL factorMotion;        //zREAL        //nicht zu klein machen. Sonst: Freeze bei hoher Framerate!
	zREAL frameTimeFloat;      //zREAL [msec] //Zeit der zwischen diesem und dem letzten Frame verstrichen ist
	zREAL totalTimeFloat;      //zREAL [msec] //gesamte Zeit
	zREAL frameTimeFloatSecs;  //zREAL  [s]
	zREAL totalTimeFloatSecs;  //zREAL  [s]
	zDWORD lastTimer;           //zDWORD
	zDWORD frameTime;           //zDWORD [msec] //nochmal als Ganzahl
	zDWORD totalTime;           //zDWORD [msec]
	zDWORD minFrameTime;        //zDWORD       //antifreeze. Sonst wird die Framezeit auf 0 gerundet und nichts bewegt sich

	zDWORD forcedMaxFrameTime;  //zDWORD //länger als das darf ein Frame (in Spielzeit) nicht dauern. Um zu große Zeitsprünge für die Objekte zu vermeiden? Jedenfalls sort dies dafür, dass das Spiel langsamer läuft, wenn das Spiel mit rendern nicht hinterherkommt.


	//.text:006370B0 ; public: void __thiscall zCTimer::SetMotionFactor(float)
	void SetMotionFactor(float p1)
	{
		XCALL(0x006370B0);
	};

	//.data:0099B3D4 ; class zCTimer ztimer
	static zCTimer* GetTimer()
	{
		return (zCTimer*)0x0099B3D4;
	};
};

#undef __G2EXT_API_HEADER

#endif  //__ZTIMER_H_INCLUDED__
