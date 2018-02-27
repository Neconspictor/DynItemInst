#pragma once

#include <vector>
#include <api/g2/zstring.h>
#include <Util_Constants.h>
#include <api/g2/oCObjectFactory.h>

class zCVisual
{
public:

	enum EVisualType
	{
		VT_OTHER,
		VT_PROGMESHPROTO,
		VT_MODEL,
		VT_PARTICLE_FX,
		VT_MORPHMESH,
		VT_DECAL,
	};

	static std::string stringOfEVisualType(enum EVisualType type)
	{
		switch (type) {
		case VT_OTHER: return "VT_OTHER";
		case VT_PROGMESHPROTO: return "VT_PROGMESHPROTO";
		case VT_MODEL: return "VT_MODEL";
		case VT_PARTICLE_FX: return "VT_PARTICLE_FX";
		case VT_MORPHMESH: return "VT_MORPHMESH";
		case VT_DECAL: return "VT_DECAL";
		default: return "Unknown";
		}
	}

	/** File extension this visual uses. Handy for finding out what class this is */
	const char* GetFileExtension(int i)
	{
		if (__GetFileExtension(i))
			return ((zSTRING*)__GetFileExtension(i))->ToChar();

		return "";
	}

	const char* GetObjectName()
	{
		return __GetObjectName().ToChar();
	}

	/** Returns the class-type of this visual */
	EVisualType GetVisualType()
	{
		std::vector<std::string> extv;

		int e = 0;
		while (strlen(GetFileExtension(e)) > 0)
		{
			extv.push_back(GetFileExtension(e));
			e++;
		}

		for (unsigned int i = 0; i<extv.size(); i++)
		{
			std::string ext = extv[i];

			if (ext == ".3DS")
				return VT_PROGMESHPROTO;
			else if (ext == ".MMS")
				return VT_MORPHMESH;
			else if (ext == ".MDS" || ext == ".ASC")
				return VT_MODEL;
			else if (ext == ".PFX")
				return VT_PARTICLE_FX;
			else if (ext == ".TGA")
				return VT_DECAL;
		}

		return VT_OTHER;
	}

private:
	zSTRING& __GetObjectName()
	{
		XCALL(GothicMemoryLocations::zCObject::GetObjectName);
	}

	const zSTRING* __GetFileExtension(int i)
	{
		int* vtbl = (int*)((int*)this)[0];

		constants::ZCVisualGetFileExtension fn = (constants::ZCVisualGetFileExtension)vtbl[GothicMemoryLocations::zCVisual::VTBL_GetFileExtension];
		return fn(this, i);

	}
};

//.text:00567600 public: static class zCMesh * __cdecl zCMesh::Load(class zSTRING const &, int) proc near
static void* __cdecl LoadMesh(zSTRING const&, int)
{
	XCALL(0x00567600);
}