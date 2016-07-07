#include "PortalDrawDistance.h"
#include <Windows.h>
#include <Util.h>


/*
FROM Magoth (thanks!):

//////////////////////////////
//GOTHIC II NOTR Gothic2.exe//
//////////////////////////////

[OutDoorPortalDistanceMultiplier]
Addr = "0x0052F25E"
Type = "float"
NewVar = "PARAMETERS:OutDoorPortalDistanceMultiplier"
NewMul = "3240000"

[OutDoorPortalDistanceMultiplier1]
Addr = "0x0052F266"
Type = "float"
NewVar = "PARAMETERS:OutDoorPortalDistanceMultiplier"
NewMul = "3240000"
NewAdd = "-1800000"

[InDoorPortalDistanceMultiplier]
Addr = "0x005348A9"
Type = "float"
NewVar = "PARAMETERS:InDoorPortalDistanceMultiplier"
NewMul = "3240000"

[InDoorPortalDistanceMultiplier1]
Addr = "0x005348B1"
Type = "float"
NewVar = "PARAMETERS:InDoorPortalDistanceMultiplier"
NewMul = "3240000"
NewAdd = "-1800000"

*/

PortalDrawDistance::PortalDrawDistance()
{
	moduleDesc = "PortalDrawDistance";
}

PortalDrawDistance::~PortalDrawDistance()
{
}

void PortalDrawDistance::hookModule()
{
	changeOutDoorPortalDistanceMulitplier1();
	changeOutDoorPortalDistanceMulitplier2();
	changeInDoorPortalDistanceMultiplier1();
	changeInDoorPortalDistanceMultiplier2();
}

void PortalDrawDistance::unHookModule()
{
}

void PortalDrawDistance::changeOutDoorPortalDistanceMulitplier1()
{
	DWORD old;
	LPVOID address = (LPVOID)(0x0052F25E);
	VirtualProtect(address, 4, PAGE_EXECUTE_READWRITE, &old);
	float defaultDrawDistance = 3240000.0f; //0x4a45c100 at address 0x0052F25A
	float multiplier = 10.0f;


	//.text:0052F25A                 mov     [esp+78h+var_5C], 4A45C100h
	float* portalDistance = (float*)0x0052F25E; //+4 since mov instruction has 4 bytes!

													  //only assign new portal draw distance, if address hasn't changed yet
	float readValue = *portalDistance;
	if (readValue != defaultDrawDistance)
	{
		logStream << "PortalDrawDistance::changeOutDoorPortalDistanceMulitplier: won't change anything!" << std::endl;
		util::logAlways(&logStream);
		return;
	}

	*portalDistance = defaultDrawDistance * multiplier;
	//VirtualProtect(address, 4, old, &old);
	logStream << "PortalDrawDistance::changeOutDoorPortalDistanceMulitplier: success!" << std::endl;
	util::logAlways(&logStream);
}

void PortalDrawDistance::changeOutDoorPortalDistanceMulitplier2()
{
	DWORD old;
	LPVOID address = (LPVOID)(0x0052F266);
	VirtualProtect(address, 4, PAGE_EXECUTE_READWRITE, &old);

	float defaultDrawDistance = 3240000.0f; //0x4a45c100 at address 0x0052F25A
	float defaultDrawDistance2 = 1440000.0f; //0x49afc800 at address 0x0052F262
	float multiplier = 10.0f;
	float difference = 1800000; // 3240000.0f - 1440000.0f

								//.text:0052F262                 mov     [esp+78h+var_64], 49AFC800h
	float* portalDistance = (float*)0x0052F266; //+4 since mov instruction has 4 bytes!

											   //only assign new portal distance if address hasn't changed yet!
	float readValue = *portalDistance;
	if (readValue != defaultDrawDistance2)
	{
		logStream << "PortalDrawDistance::changeOutDoorPortalDistanceMulitplier2: won't change anything!" << std::endl;
		util::logAlways(&logStream);
		return;
	}

	*portalDistance = defaultDrawDistance * multiplier - difference;
	//VirtualProtect(address, 4, old, &old);
	logStream << "PortalDrawDistance::changeOutDoorPortalDistanceMulitplier2: success!" << std::endl;
	util::logAlways(&logStream);
}

void PortalDrawDistance::changeInDoorPortalDistanceMultiplier1()
{
	DWORD old;
	LPVOID address = (LPVOID)(0x005348A9);
	VirtualProtect(address, 4, PAGE_EXECUTE_READWRITE, &old);
	float defaultDrawDistance = 3240000.0f; //0x4a45c100 at address 0x0052F25A
	float multiplier = 10.0f;


	//.text:0052F25A                 mov     [esp+78h+var_5C], 4A45C100h
	float* portalDistance = (float*)address; //+4 since mov instruction has 4 bytes!

												//only assign new portal draw distance, if address hasn't changed yet
	float readValue = *portalDistance;
	if (readValue != defaultDrawDistance)
	{
		logStream << "PortalDrawDistance::changeInDoorPortalDistanceMulitplier: won't change anything!" << std::endl;
		util::logAlways(&logStream);
		return;
	}

	*portalDistance = defaultDrawDistance * multiplier;
	//VirtualProtect(address, 4, old, &old);
	logStream << "PortalDrawDistance::changeInDoorPortalDistanceMulitplier: success!" << std::endl;
	util::logAlways(&logStream);
}

void PortalDrawDistance::changeInDoorPortalDistanceMultiplier2()
{
	DWORD old;
	LPVOID address = (LPVOID)(0x005348B1);
	VirtualProtect(address, 4, PAGE_EXECUTE_READWRITE, &old);

	float defaultDrawDistance = 3240000.0f; //0x4a45c100 at address 0x0052F25A
	float defaultDrawDistance2 = 1440000.0f; //0x49afc800 at address 0x0052F262
	float multiplier = 10.0f;
	float difference = 1800000; // 3240000.0f - 1440000.0f

								//.text:0052F262                 mov     [esp+78h+var_64], 49AFC800h
	float* portalDistance = (float*)address; //+4 since mov instruction has 4 bytes!

												//only assign new portal distance if address hasn't changed yet!
	float readValue = *portalDistance;
	if (readValue != defaultDrawDistance2)
	{
		logStream << "PortalDrawDistance::changeInDoorPortalDistanceMulitplier2: won't change anything!" << std::endl;
		util::logAlways(&logStream);
		return;
	}

	*portalDistance = defaultDrawDistance * multiplier - difference;
	//VirtualProtect(address, 4, old, &old);
	logStream << "PortalDrawDistance::changeInDoorPortalDistanceMulitplier2: success!" << std::endl;
	util::logAlways(&logStream);
}
