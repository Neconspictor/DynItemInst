#pragma once

#include "zCVisual.h"
#include "zCModelTexAniState.h"
#include "zCMeshSoftSkin.h"
#include "zTypes.h"
#include <map>
#include <D3DX10math.h>

struct zCModelNode;
struct zCModelNodeInst
{
	zCModelNodeInst* ParentNode;
	zCModelNode* ProtoNode;
	zCVisual* NodeVisual;
	D3DXMATRIX  Trafo;
	D3DXMATRIX TrafoObjToCam;
	zTBBox3D BBox3D;

	zCModelTexAniState TexAniState;
};



struct zCModelNode
{
	zCModelNode* ParentNode;
	zSTRING	NodeName;
	zCVisual* Visual;
	D3DXMATRIX Transform;

	zVEC3 NodeRotAxis;
	float NodeRotAngle;
	zVEC3	Translation;
	D3DXMATRIX TransformObjToWorld;
	D3DXMATRIX* NodeTransformList;
	zCModelNodeInst* LastInstNode;
};

struct zTMdl_NodeVobAttachment
{
	zCVob* Vob;
	zCModelNodeInst* NodeInst;
};

class zCModelMeshLib;
struct zTMeshLibEntry
{
	zCModelTexAniState TexAniState;
	zCModelMeshLib* MeshLibPtr;
};

class zCModelAni
{
public:
	bool IsIdleAni()
	{
		DWORD value = *(DWORD *)THISPTR_OFFSET(GothicMemoryLocations::zCModelAni::Offset_Flags);
		return (value & GothicMemoryLocations::zCModelAni::Mask_FlagIdle) != 0;
	}

	int GetNumAniFrames()
	{
		return *(uint16_t *)THISPTR_OFFSET(GothicMemoryLocations::zCModelAni::Offset_NumFrames);
	}
};

class zCModelAniActive
{
public:
	zCModelAni* protoAni;
	zCModelAni* nextAni;
};

class zCModelMeshLib : public zCObject
{
public:
	struct zTNodeMesh
	{
		zCVisual* Visual;
		int NodeIndex;
	};

	/** This returns the list of nodes which hold information about the bones and attachments later */
	zCArray<zTNodeMesh>* GetNodeList()
	{
		return &NodeList;
	}

	/** Returns the list of meshes which store the vertex-positions and weights */
	zCArray<zCMeshSoftSkin *>* GetMeshSoftSkinList()
	{
		return &SoftSkinList;
	}

	const char* GetVisualName()
	{
		if (GetMeshSoftSkinList()->m_numInArray > 0)
			return GetMeshSoftSkinList()->m_array[0]->GetObjectName();

		return "";
	}

private:
	zCArray<zTNodeMesh>			NodeList;
	zCArray<zCMeshSoftSkin*>	SoftSkinList;
};

class zCModelPrototype
{
public:

	/** This returns the list of nodes which hold information about the bones and attachments later */
	zCArray<zCModelNode *>* GetNodeList()
	{
		return (zCArray<zCModelNode *> *)THISPTR_OFFSET(GothicMemoryLocations::zCModelPrototype::Offset_NodeList);
	}

	/** Returns the list of meshes which store the vertex-positions and weights */
	zCArray<zCMeshSoftSkin *>* GetMeshSoftSkinList()
	{
		return (zCArray<zCMeshSoftSkin *> *)THISPTR_OFFSET(GothicMemoryLocations::zCModelPrototype::Offset_MeshSoftSkinList);
	}

	/** Returns the name of the first Mesh inside this */
	const char* GetVisualName()
	{
		if (GetMeshSoftSkinList()->m_numInArray > 0)
			return GetMeshSoftSkinList()->m_array[0]->GetObjectName();

		return "";
	}
};

/** Insert description. */
class zCModel : public zCVisual
{
public:
	//
	/** This method closes the loadscreen.
	* @usable Ingame only
	*/
	void StartAni(int a, int b) { XCALL(0x0057B070); };

	//
	/** This method closes the loadscreen.
	* @usable Ingame only
	*/
	void StartAni(const zSTRING& pA, int pB) { XCALL(0x0057AF70); };

	//
	/** This method closes the loadscreen.
	* @usable Ingame only
	*/
	void StopAni(int a) { XCALL(0x0057ABE0); };

	//
	/** This method closes the loadscreen.
	* @usable Ingame only
	*/
	void FadeOutAni(int pA) { XCALL(0x0057EF50); };

	//
	/** This method closes the loadscreen.
	* @usable Ingame only
	*/
	void FadeOutAnisLayerRange(int a, int b) { XCALL(0x0057F1F0); };

	//
	/** This method closes the loadscreen.
	* @usable Ingame only
	*/
	void StopAnisLayerRange(int pA, int pB) { XCALL(0x0057F240); };

	/** Creates an array of matrices for the bone transforms */
	void __fastcall RenderNodeList(zTRenderContext& renderContext, zCArray<zMAT4*>& boneTransforms, zCRenderLightContainer& lightContainer, int lightingMode = 0)
	{
		XCALL(GothicMemoryLocations::zCModel::RenderNodeList);
	}

	/** Returns the current amount of active animations */
	int GetNumActiveAnimations()
	{
		return *(int *)THISPTR_OFFSET(GothicMemoryLocations::zCModel::Offset_NumActiveAnis);
	}

	/** Returns true if only an idle-animation is running */
	bool IdleAnimationRunning()
	{
		zCModelAniActive* activeAni = *(zCModelAniActive **)THISPTR_OFFSET(GothicMemoryLocations::zCModel::Offset_AniChannels);

		return GetNumActiveAnimations() == 1 && (/*activeAni->protoAni->IsIdleAni() ||*/ activeAni->protoAni->GetNumAniFrames() <= 1);
	}

	/** This is needed for the animations to work at full framerate */
	void SetDistanceToCamera(float dist)
	{
		*(float *)THISPTR_OFFSET(GothicMemoryLocations::zCModel::Offset_DistanceModelToCamera) = dist;
	}

	/** Updates stuff like blinking eyes, etc */
	void zCModel::UpdateMeshLibTexAniState()
	{
		for (int i = 0; i<GetMeshLibList()->m_numInArray; i++)
			GetMeshLibList()->m_array[i]->TexAniState.UpdateTexList();
	}

	int GetIsVisible()
	{
		return (*(int *)THISPTR_OFFSET(GothicMemoryLocations::zCModel::Offset_Flags)) & 1;
	}

	void SetIsVisible(bool visible)
	{
		int v = visible ? 1 : 0;

		BYTE* flags = (BYTE *)THISPTR_OFFSET(GothicMemoryLocations::zCModel::Offset_Flags);

		*flags &= ~1;
		*flags |= v;
	}

	zVEC3 GetModelScale()
	{
		return *(zVEC3 *)THISPTR_OFFSET(GothicMemoryLocations::zCModel::Offset_ModelScale);
	}

	float GetModelFatness()
	{
		return *(float *)THISPTR_OFFSET(GothicMemoryLocations::zCModel::Offset_ModelFatness);
	}

	int GetDrawHandVisualsOnly()
	{
		return *(int *)THISPTR_OFFSET(GothicMemoryLocations::zCModel::Offset_DrawHandVisualsOnly);
	}

	zCArray<zCModelNodeInst *>* GetNodeList()
	{
		return (zCArray<zCModelNodeInst *> *)THISPTR_OFFSET(GothicMemoryLocations::zCModel::Offset_NodeList);
	}

	zCArray<zCMeshSoftSkin *>* GetMeshSoftSkinList()
	{
		return (zCArray<zCMeshSoftSkin *> *)THISPTR_OFFSET(GothicMemoryLocations::zCModel::Offset_MeshSoftSkinList);
	}

	zCArray<zCModelPrototype *>* GetModelProtoList()
	{
		return (zCArray<zCModelPrototype *> *)THISPTR_OFFSET(GothicMemoryLocations::zCModel::Offset_ModelProtoList);
	}

	zCArray<zTMeshLibEntry *>* GetMeshLibList()
	{
		return (zCArray<zTMeshLibEntry *> *)THISPTR_OFFSET(GothicMemoryLocations::zCModel::Offset_MeshLibList);
	}

	zCArray<zTMdl_NodeVobAttachment>* GetAttachedVobList()
	{
		return (zCArray<zTMdl_NodeVobAttachment> *)THISPTR_OFFSET(GothicMemoryLocations::zCModel::Offset_AttachedVobList);
	}

	/* Updates the world matrices of the attached VOBs */
	void UpdateAttachedVobs()
	{
		XCALL(GothicMemoryLocations::zCModel::UpdateAttachedVobs);
	}

	/** Fills a vector of (viewspace) bone-transformation matrices for this frame */
	void GetBoneTransforms(std::vector<D3DXMATRIX>* transforms, zCVob* vob = NULL)
	{
		if (!GetNodeList())
			return;

		// Make this static so we don't reallocate the memory every time
		static std::vector<D3DXMATRIX*> tptr;
		tptr.resize(GetNodeList()->m_numInArray, NULL);
		for (int i = 0; i<GetNodeList()->m_numInArray; i++)
		{
			zCModelNodeInst* node = GetNodeList()->m_array[i];
			zCModelNodeInst* parent = node->ParentNode;
			tptr[i] = &node->TrafoObjToCam;

			// Calculate transform for this node
			if (parent)
			{
				node->TrafoObjToCam = parent->TrafoObjToCam * node->Trafo;
			}
			else
			{
				node->TrafoObjToCam = node->Trafo;
			}

		}
		// Put them into our vector
		for (unsigned int i = 0; i<tptr.size(); i++)
		{
			D3DXMATRIX m = *tptr[i];
			transforms->push_back(m);
		}
	}

	/** Fills a vector of worldspace bone-transformation matrices for this frame */
	void GetBoneWorldTransforms(std::vector<D3DXMATRIX>* transforms)
	{
		if (!GetNodeList())
			return;

		static std::map<zCModelNodeInst*, int> nodeIndexMap;
		transforms->resize(GetNodeList()->m_numInArray);

		//fill index map
		for (int i = 0; i < GetNodeList()->GetSize(); ++i)
		{
			nodeIndexMap.insert(std::pair<zCModelNodeInst*, int>(GetNodeList()->m_array[i], i));
		}

		BYTE* camera = (BYTE*)0x008D7F94;

		zMAT4 cameraTrafo = *(zMAT4*)(camera + 164);

		//reverse view space projection
		for (int i = 0; i<GetNodeList()->m_numInArray; i++)
		{
			zCModelNodeInst* node = GetNodeList()->m_array[i];
			(*transforms)[i] = node->Trafo;
		}

		for (int i = 0; i<GetNodeList()->m_numInArray; i++)
		{
			zCModelNodeInst* node = GetNodeList()->m_array[i];
			zCModelNodeInst* parent = node->ParentNode;

			// Calculate transform for this node
			if (parent)
			{
				//node->TrafoObjToCam = parent->TrafoObjToCam * node->Trafo;
				int parentIndex = nodeIndexMap.at(parent);
				(*transforms)[i] = (*transforms)[parentIndex] * (*transforms)[i];
			} else
			{
				//(*transforms)[i] = node->Trafo;
			}
		}
	}

	const char* GetVisualName()
	{
		if (GetMeshSoftSkinList()->m_numInArray > 0)
			return GetMeshSoftSkinList()->m_array[0]->GetObjectName();

		return "";
		//return __GetVisualName().ToChar();
	}

	zSTRING GetModelName()
	{
		XCALL(GothicMemoryLocations::zCModel::GetVisualName);
	}

private:
};