#pragma once
#include <WinDef.h>

class zCVertex
{
public:
	/*#ifdef BUILD_GOTHIC_1_08k
	int id;
	#endif*/

	float Position[3];

	int TransformedIndex;
	int MyIndex;
};

class zCVertFeature
{
public:
	float normal[3];
	DWORD lightStatic;
	DWORD lightDynamic;
	float texCoord[2];
};