#pragma once

#pragma pack (push, 1)
#include "zCProgMeshProto.h"

struct zTWeightEntry {
	float Weight;
	zVEC3	VertexPosition;
	unsigned char NodeIndex;
};
#pragma pack (pop)

class zCOBBox3D;
class zCMeshSoftSkin : public zCProgMeshProto
{
public:

	/*zCArray<int>* GetNodeIndexList()
	{
	return (zCArray<int> *)THISPTR_OFFSET(GothicMemoryLocations::zCMeshSoftSkin::Offset_NodeIndexList);
	}*/

	struct zTNodeWedgeNormal {
		zVEC3		Normal;
		int			NodeIndex;
	};

	char* GetVertWeightStream()
	{
		return *(char **)THISPTR_OFFSET(GothicMemoryLocations::zCMeshSoftSkin::Offset_VertWeightStream);
	};

};