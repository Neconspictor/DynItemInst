#pragma once
#include "Util_Constants.h"
#include "zCArrayAdapt.h"
#include "zCVisual.h"

typedef unsigned short VERTEX_INDEX;

struct zTPMTriangle {
	VERTEX_INDEX edge[3];
};

struct zTPMWedge {
	zVEC3 normal;
	zVEC2 texUV;
	VERTEX_INDEX position;
};

struct zTPMTriangleEdges
{
	VERTEX_INDEX edge[3];
};

struct zTPMEdge
{
	VERTEX_INDEX wedge[2];
};

struct zTPMVertexUpdate
{
	VERTEX_INDEX numNewTri;
	VERTEX_INDEX numNewWedge;
};

struct zTPlane
{
	float Distance;
	zVEC3 Normal;
};



class zCSubMesh {
public:
	zCMaterial*						Material;
	zCArrayAdapt<zTPMTriangle>		TriList;
	zCArrayAdapt<zTPMWedge>			WedgeList;
	zCArrayAdapt<float>				ColorList;
	zCArrayAdapt<VERTEX_INDEX>	TriPlaneIndexList;
	zCArrayAdapt<zTPlane>			TriPlaneList;
	zCArrayAdapt<zTPMTriangleEdges>	TriEdgeList;
	zCArrayAdapt<zTPMEdge>			EdgeList;
	zCArrayAdapt<float>				EdgeScoreList;

	zCArrayAdapt<VERTEX_INDEX>	WedgeMap;
	zCArrayAdapt<zTPMVertexUpdate>	VertexUpdates;

	int vbStartIndex;

	static const unsigned int CLASS_SIZE = 0x58;
};


class zCProgMeshProto : public zCVisual
{
public:

	zCArrayAdapt<zVEC3>* GetPositionList()
	{
		return (zCArrayAdapt<zVEC3> *)THISPTR_OFFSET(GothicMemoryLocations::zCProgMeshProto::Offset_PositionList);
	}

	zCSubMesh* GetSubmeshes()
	{
		return *(zCSubMesh **)(((char *)this) + (GothicMemoryLocations::zCProgMeshProto::Offset_Submeshes));
	}

	zCSubMesh* GetSubmesh(int n)
	{
		return (zCSubMesh *)(((char *)GetSubmeshes()) + zCSubMesh::CLASS_SIZE * n);
	}

	int GetNumSubmeshes()
	{
		return *(int *)(((char *)this) + (GothicMemoryLocations::zCProgMeshProto::Offset_NumSubmeshes));
	}

	//.text:005C7180 public: static class zCVisual * __cdecl zCProgMeshProto::Load(class zSTRING const &) proc near
	static zCVisual* __cdecl Load(zSTRING const& filename)
	{
		XCALL(0x005C7180);
	}
};