#pragma once

#include <api/g2/macros.h>
#include <api/g2/zcview.h>

class zCModelTexAniState
{
public:
	void UpdateTexList()
	{
		XCALL(GothicMemoryLocations::zCModelTexAniState::UpdateTexList);
	}

	int	NumNodeTex;
	zCTexture** NodeTexList;
	int	ActAniFrames[8];
};