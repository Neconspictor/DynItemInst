#pragma once

#include <api/interface.h>

#include <api/g2/zcmaterial.h>
#include <api/g2/zcvertex.h>

class zCPolygon
{
public:
	zCVertex** getVertices() const
	{
		return *(zCVertex***)THISPTR_OFFSET(Offset_VerticesArray);
	}

	zVEC3* getPolyNormal() const
	{
		return (zVEC3*)THISPTR_OFFSET(GothicMemoryLocations::zCPolygon::Offset_PolyNormalX);
	}

	zCMaterial* GetMaterial() const
	{
		return *(zCMaterial**)THISPTR_OFFSET(GothicMemoryLocations::zCPolygon::Offset_Material);
	}

	unsigned char GetNumPolyVertices() const
	{
		return *(unsigned char*)THISPTR_OFFSET(GothicMemoryLocations::zCPolygon::Offset_NumPolyVertices);
	}

	//.text:005BBA70 public: int __fastcall zCPolygon::CheckBBoxPolyIntersection(struct zTBBox3D const &)const proc near
	int __fastcall CheckBBoxPolyIntersection(zTBBox3D const&) const
	{
		XCALL(0x005BBA70);
	}

	//.text:005B8180 public: int __thiscall zCPolygon::CalcNormal(void) proc near
	int __thiscall CalcNormal()
	{
		XCALL(0x005B8180);
	}

	//


public:
	static const unsigned int Offset_VerticesArray = 0x00;
	static const unsigned int Offset_PolyNormalX = 0x0C;
	static const unsigned int Offset_PolyNormalY = 0x10;
	static const unsigned int Offset_PolyNormalZ = 0x14;
	static const unsigned int Offset_FeaturesArray = 0x2C;
	static const unsigned int Offset_NumPolyVertices = 0x30;
	static const unsigned int Offset_PolyFlags = 0x31;
	static const unsigned int Offset_Material = 0x18;
	static const unsigned int Offset_Lightmap = 0x1C;

private:
	char data[56];
};


/*  From Degenerated */
struct zCPolygonOffsets
{
	static const unsigned int Offset_VerticesArray = 0x00;
	static const unsigned int Offset_PolyNormalX = 0x0C;
	static const unsigned int Offset_PolyNormalY = 0x10;
	static const unsigned int Offset_PolyNormalZ = 0x14;
	static const unsigned int Offset_Material = 0x18;
	static const unsigned int Offset_Lightmap = 0x1C;
	static const unsigned int Offset_FeaturesArray = 0x2C;
	static const unsigned int Offset_NumPolyVertices = 0x30;
	static const unsigned int Offset_PolyFlags = 0x31;
};

struct PolyFlags {
	unsigned char PortalPoly : 2;
	unsigned char Occluder : 1;
	unsigned char SectorPoly : 1;
	unsigned char MustRelight : 1;
	unsigned char PortalIndoorOutdoor : 1;
	unsigned char GhostOccluder : 1;
	unsigned char NoDynLightNear : 1;
	//VERTEX_INDEX SectorIndex : 16;
	unsigned SectorIndex : 16;
};