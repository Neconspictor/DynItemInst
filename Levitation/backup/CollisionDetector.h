#pragma once
#include <api/g2/ocnpc.h>
#include "Util_Constants.h"
#include "zCArrayAdapt.h"
#include <d3dx10math.h>
#include "zCModelExtended.h"

class CollisionDetector
{
public:
	static bool __cdecl detectCollision(oCNpc* npc, zMAT4& mat);

	static zVEC3 __cdecl getCollidingPolyNormal(oCNpc* npc, const zMAT4& mat);

private:
	static bool detectStaticCollision(oCNpc* npc, zMAT4& mat);
	static bool detectDynamicCollision(oCNpc* npc, zMAT4& mat);
	static bool vobIsInTouchList(zCVob* source, zCVob* vobToCheck);
	static bool checkIntersection_zCProgMeshProto(void* progMesh, zMAT4 const & mat, const zTBBox3D& boundingBox);
	static zMAT4 inverse(zMAT4 const&);
	static zMAT4 transpose(zMAT4 const& m);
	static float det(zMAT4 const&);
	static float det(zMAT3 const&);
	static zMAT3 getMatrixMinor(zMAT4 const& m, int i, int j);
	static zMAT4 getCofactorMatrix(zMAT4 const &);
	static void doStatistics(oCNpc* npc);
};

class Box
{
public:

	struct SimpleBox
	{
		zVEC3 local00;
		zVEC3 local01;
		zVEC3 local10;
		zVEC3 local11;

		zVEC3 parent00;
		zVEC3 parent01;
		zVEC3 parent10;
		zVEC3 parent11;
	};

	zVEC3 middlePoint;
	// coordination system with scaling factors
	zVEC3 up, right, at;
	zCOLOR color;
	D3DXMATRIX orientation;
	D3DXMATRIX localInv;
	D3DXMATRIX parentTrafo;
	D3DXMATRIX rotation;
	zVEC3 translation;
	zVEC3 parentOffset;
	zVEC3 parentTrans;
	bool hasParent;
	bool drawBox;
	SimpleBox bbox;

	Box(zVEC3 minPos, zVEC3 up, zVEC3 right, zVEC3 at, zCOLOR color)
	{
		this->middlePoint = minPos + up/2 + right/2 + at/2;
		this->up = up;
		this->right = right;
		this->at = at;
		this->color = color;
		D3DXMatrixIdentity(&orientation);
		translation = zVEC3(0,0,0);
	}

	Box(zTBBox3D const & box, zCModelNodeInst* node, zCOLOR col)
	{
		float x = box.bbox3D_maxs.x - box.bbox3D_mins.x;
		float y = box.bbox3D_maxs.y - box.bbox3D_mins.y;
		float z = box.bbox3D_maxs.z - box.bbox3D_mins.z;
		up = zVEC3(0, y, 0);
		right = zVEC3(x, 0, 0);
		at = zVEC3(0,0, z);
		middlePoint = zVEC3(0, 0, 0);
		D3DXMatrixIdentity(&orientation);
		D3DXMatrixIdentity(&rotation);
		translation = box.bbox3D_mins + up / 2 + right / 2 + at / 2;
		color = col;

		D3DXMATRIX trafo = node->Trafo;
		parentOffset = zVEC3(trafo._14, trafo._24, trafo._34);
		trafo._14 = trafo._24 = trafo._34 = 0;
		D3DXMatrixInverse(&localInv, NULL, &trafo);

		D3DXMatrixIdentity(&parentTrafo);
		hasParent = true;
		drawBox = true;

		bbox.local00 = zVEC3(-20, -90, 15);
		bbox.local01 = zVEC3(20, -90, 15);
		bbox.local10 = zVEC3(-20, -90, -15);
		bbox.local11 = zVEC3(20, -90, -15);

		bbox.parent00 = zVEC3(-20, 90, 15);
		bbox.parent01 = zVEC3(20, 90, 15);
		bbox.parent10 = zVEC3(-20, 90, -15);
		bbox.parent11 = zVEC3(20, 90, -15);
	}

	void setParentTrafo(D3DXMATRIX const & mat)
	{
		parentTrafo = mat;
	}

	void setParentTrans(zVEC3 const& vec)
	{
		parentTrans = vec;
	}

	void setTransformation(D3DXMATRIX const & mat)
	{
		orientation = mat;
		setTranslation(&orientation, zVEC3(0,0,0));
		translation = getTranslation(mat);
	}

	void setOrientation(D3DXMATRIX const & mat)
	{
		orientation = mat;
		setTranslation(&orientation, zVEC3(0, 0, 0));
		//D3DXMatrixInverse(&orientation, NULL, &orientation);
	}

	D3DXMATRIX getTransformation() const
	{
		D3DXMATRIX result = orientation;
		setTranslation(&result, translation);
		return result;
	}

	D3DXVECTOR3 toD3DVector(zVEC3 const & vec) const
	{
		return D3DXVECTOR3(vec.x, vec.y, vec.z);
	}

	void setHasParent(bool value)
	{
		hasParent = value;
	}

	void draw()
	{  	
		zVEC3 upCpy, rightCpy, atCpy, middleCpy, parentOffsetCpy, minPos;
		minPos = middlePoint - (up / 2) - (right / 2) - (at / 2);
		//rotate by orientation
		D3DXMATRIX rot = orientation;
		D3DXMATRIX toParent = parentTrafo;
		upCpy = up;
		convertPointToWorld(&upCpy, rot, zVEC3(0, 0, 0));
		rightCpy = right;
		convertPointToWorld(&rightCpy, rot, zVEC3(0, 0, 0));
		atCpy = at;
		convertPointToWorld(&atCpy, rot, zVEC3(0, 0, 0));
		middleCpy = middlePoint;
		convertPointToWorld(&middleCpy, rot, translation);
		convertPointToWorld(&minPos, rot, translation);

		//translate position vectors
		parentOffsetCpy = parentTrans;


		zVEC3 current = minPos;
		if (drawBox)
		{
			SimpleBox boxCpy = bbox;
			convertToWorld(&boxCpy, orientation, translation);
			drawSimpleBox(boxCpy);
		}


		/*constants::drawLine(current, current + upCpy, color, 0);
		constants::drawLine(current, current + rightCpy, color, 0);
		constants::drawLine(current, current + atCpy, color, 0);

		current = minPos + upCpy;
		constants::drawLine(current, current + rightCpy, color, 0);
		constants::drawLine(current, current + atCpy, color, 0);

		current = minPos + rightCpy;
		constants::drawLine(current, current + upCpy, color, 0);
		constants::drawLine(current, current + atCpy, color, 0);

		current = minPos + atCpy;
		constants::drawLine(current, current + upCpy, color, 0);
		constants::drawLine(current, current + rightCpy, color, 0);

		current = minPos + upCpy + atCpy;
		constants::drawLine(current, current + rightCpy, color, 0);

		current = minPos + upCpy + rightCpy;
		constants::drawLine(current, current + atCpy, color, 0);

		current = minPos + atCpy + rightCpy;
		constants::drawLine(current, current + upCpy, color, 0);*/

		//draw connection to parent node in red
		if (hasParent)
		{
			constants::drawLine(middleCpy, parentOffsetCpy, zCOLOR(255, 0, 255), 0);
		}
	}

	void drawSimpleBox(SimpleBox const& box)  const
	{
		// connect from local to parent plane
		constants::drawLine(box.local00, box.parent00, color, 1);
		constants::drawLine(box.local01, box.parent01, color, 1);
		constants::drawLine(box.local10, box.parent10, color, 1);
		constants::drawLine(box.local11, box.parent11, color, 1);

		//local plane
		constants::drawLine(box.local00, box.local01, color, 1);
		constants::drawLine(box.local01, box.local11, color, 1);
		constants::drawLine(box.local11, box.local10, color, 1);
		constants::drawLine(box.local10, box.local00, color, 1);

		//parent plane
		constants::drawLine(box.parent00, box.parent01, color, 1);
		constants::drawLine(box.parent01, box.parent11, color, 1);
		constants::drawLine(box.parent11, box.parent10, color, 1);
		constants::drawLine(box.parent10, box.parent00, color, 1);
	}

	void convertToWorld(SimpleBox* box, D3DXMATRIX const& mat, zVEC3 const& translation)
	{
		convertPointToWorld(&box->local00, mat, translation);
		convertPointToWorld(&box->local01, mat, translation);
		convertPointToWorld(&box->local10, mat, translation);
		convertPointToWorld(&box->local11, mat, translation);

		convertPointToWorld(&box->parent00, mat, translation);
		convertPointToWorld(&box->parent01, mat, translation);
		convertPointToWorld(&box->parent10, mat, translation);
		convertPointToWorld(&box->parent11, mat, translation);
	}

	void convertPointToWorld(zVEC3* point, D3DXMATRIX const& mat, zVEC3 const& translation)
	{
		D3DXVECTOR3 temp;
		D3DXVec3TransformCoord(&temp, &toD3DVector(*point), &mat);
		*point = zVEC3(temp.x, temp.y, temp.z);
		*point += translation;
	}


	static void setTranslation(D3DXMATRIX* mat, zVEC3 const& vec)
	{
	mat->_14 = vec.x;
	mat->_24 = vec.y;
	mat->_34 = vec.z;
	}

	void setTranslation(zVEC3 const& vec)
	{
		translation.x = vec.x;
		translation.y = vec.y;
		translation.z = vec.z;
	}

	zVEC3 getTranslation(D3DXMATRIX const& mat) const
	{
		return zVEC3(mat._14, mat._24, mat._34);
	}

	void setRotation(D3DXMATRIX const& mat)
	{
		rotation = mat;
	}
};
