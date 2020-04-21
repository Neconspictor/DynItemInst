#pragma once

#include "api/g2/ztypes.h"

constexpr int zMAT_GROUP_WATER = 5;
constexpr int zMAT_GROUP_SNOW = 6;

struct zCMaterial
{
	static const unsigned int Offset_Color = 0x38;
	static const unsigned int Offset_Texture = 0x34;
	static const unsigned int Offset_AlphaFunc = 0x74;
	static const unsigned int Offset_MatGroup = 0x40;
	static const unsigned int Offset_TexAniCtrl = 0x4C;

	static const unsigned int InitValues = 0x00564260;
	static const unsigned int Constructor = 0x00563E00;
	static const unsigned int Destructor = 0x00564070;
	static const unsigned int GetAniTexture = 0x0064BA20;


	int GetMatGroup()
	{
		return *(int*)((char*)this) + (Offset_MatGroup);
	}

	static int __thiscall IsPortalMaterial(void* pThis);

	zSTRING& GetName()
	{
		XCALL(0x0055FF10);
	}

};