#pragma once
#include <api/g2/zcvob.h>
#include <api/g2/ocgame.h>

struct zTRenderContext
{
	int ClipFlags;
	zCVob *Vob;
	zCWorld* world;
	zCCamera* cam;
	float distVobToCam;

	// More not needed stuff here
};

struct zCRenderLight
{
	int	LightType;
	zVEC3 ColorDiffuse;
	zVEC3 Position;
	zVEC3 Direction;
	float Range;
	float RangeInv;
	zVEC3 PositionLS;
	zVEC3 DirectionLS;
	float Dir_approxFalloff;
};

struct zCRenderLightContainer
{
	zCRenderLight			LightList[8];
	int						NumLights;

private:

	int	DoPrelight;
	int	DoSmoothPrelit;
	float PreLightDist;
	zMAT4 MatObjToCam;
};
