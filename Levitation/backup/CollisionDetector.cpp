#include "CollisionDetector.h"
#include <Util.h>
#include <Util_Constants.h>
#include "zCPolygon.h"
#include "zCProgMeshProto.h"
#include "zCModelExtended.h"
#include <TestModule.h>

//.text:005C8530 public: class zCMesh * __thiscall zCProgMeshProto::GetMesh(int) proc near
typedef void* (__thiscall* ZCProgMeshProtoGetMesh)(void*, int);
ZCProgMeshProtoGetMesh zCProgMeshProtoGetMesh = (ZCProgMeshProtoGetMesh)0x005C8530;

//.text:005C7130 public: virtual class zSTRING __thiscall zCProgMeshProto::GetVisualName(void) proc near
typedef zSTRING & (__thiscall* ZCProgMeshProtoGetVisualName)(void*);
ZCProgMeshProtoGetVisualName zCProgMeshProtoGetVisualName = (ZCProgMeshProtoGetVisualName)0x005A9CD0;


void transform(zMAT4 const & mat, zVEC3& vec)
{
	/*vec.x = mat._11 * vec.x + mat._12*vec.y + mat._13*vec.z + mat._14;
	vec.y = mat._21 * vec.x + mat._22*vec.y + mat._23*vec.z + mat._24;
	vec.z = mat._31 * vec.x + mat._32*vec.y + mat._33*vec.z + mat._34;*/
	zVEC3 buffer = vec;
	vec.x = mat.m[0][0] * buffer.x + mat.m[0][1] * buffer.y + mat.m[0][2] * buffer.z + mat.m[0][3];
	vec.y = mat.m[1][0] * buffer.x + mat.m[1][1] * buffer.y + mat.m[1][2] * buffer.z + mat.m[1][3];
	vec.z = mat.m[2][0] * buffer.x + mat.m[2][1] * buffer.y + mat.m[2][2] * buffer.z + mat.m[2][3];

	/*vec.x = mat.m[0][0] * buffer.x + mat.m[1][0] * buffer.y + mat.m[2][0] * buffer.z + mat.m[0][3];
	vec.y = mat.m[0][1] * buffer.x + mat.m[1][1] * buffer.y + mat.m[2][1] * buffer.z + mat.m[1][3];
	vec.z = mat.m[0][2] * buffer.x + mat.m[1][2] * buffer.y + mat.m[2][2] * buffer.z + mat.m[2][3];*/

	/*vec.x = mat._11 * vec.x + mat._21*vec.y + mat._31*vec.z;
	vec.y = mat._12 * vec.x + mat._22*vec.y + mat._32*vec.z;
	vec.z = mat._13 * vec.x + mat._23*vec.y + mat._33*vec.z;*/
};

bool CollisionDetector::detectStaticCollision(oCNpc* npc, zMAT4& mat)
{
	bool collided = false;

	zVEC3 pos(mat.m[0][3], mat.m[1][3], mat.m[2][3]);

	zCPolygon** polys;
	void* pointer = (void*)*(int*)((BYTE*)npc + 0x0b8);
	pointer = (void*)*(int*)((BYTE*)pointer + 0x1b4);
	int polyCount = 0;

	zCModel* model = npc->GetModel();
	constants::zCModelCalcModelBBox3DWorld(model);
	zTBBox3D box = constants::zCModelGetBBox3D(model);

	//make rec a little bit bigger than box
	zTBBox3D rec;
	rec.bbox3D_maxs = box.bbox3D_maxs;//zVEC3(100,100,100);
	box.bbox3D_maxs.x = 20;
	box.bbox3D_maxs.z *= 0.5;
	rec.bbox3D_mins = box.bbox3D_mins;//zVEC3(-100, -100, -100);
	box.bbox3D_mins.x = -20;
	box.bbox3D_mins.z *= 0.5;
	rec.bbox3D_maxs += pos;
	rec.bbox3D_mins += pos;

	box.bbox3D_maxs += pos;
	box.bbox3D_mins += pos;


	//static context
	constants::zCBspBaseCollectPolysInBBox3D(pointer, rec, polys, polyCount);
	if (polys != nullptr) {
		if (polyCount)
		{
			for (int i = 0; i < polyCount; ++i)
			{
				zCPolygon* poly = polys[i];

				if (poly->CheckBBoxPolyIntersection(box))
				{
					if (!collided) {
						collided = true;

						//we have found what we wanted -> a collision!
						break;
					}
				}
			}
		}
	}

	return collided;
};

std::string zVEC3ToString(zVEC3 const& vec)
{
	std::stringstream ss;
	ss << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
	return ss.str();
}

bool CollisionDetector::detectDynamicCollision(oCNpc* npc, zMAT4& mat)
{
	bool collided = false;
	// dynamic context (vobs)
	zVEC3 pos(mat.m[0][3], mat.m[1][3], mat.m[2][3]);
	zCPolygon** polys;
	void* pointer = (void*)*(int*)((BYTE*)npc + 0x0b8);
	pointer = (void*)*(int*)((BYTE*)pointer + 0x1b4);

	zCModel* model = npc->GetModel();
	constants::zCModelCalcModelBBox3DWorld(model);
	zTBBox3D box = constants::zCModelGetBBox3D(model);

	box.bbox3D_maxs.x = 20;
	box.bbox3D_maxs.z *= 0.5;
	box.bbox3D_mins.x = -20;
	box.bbox3D_mins.z *= 0.5;

	box.bbox3D_maxs += pos;
	box.bbox3D_mins += pos;

	//transform(npc->trafoObjToWorld, box.bbox3D_maxs);
	//transform(npc->trafoObjToWorld, box.bbox3D_mins);

	zCArray<zCVob*> vobArray;
	constants::zCBspBaseCollectVobsInBBox3D(pointer, vobArray, box);
	std::stringstream ss;
	static int collDetectionStatic = ((1 << 1) - 1) << 6;
	static int collDetectionDynamic = ((1 << 1) - 1) << 7;

	for (int i = 0; i < vobArray.GetSize(); ++i)
	{
		zCVob* currentVob = vobArray.GetItem(i);
		if (currentVob == npc)
		{
			//ss << "vobArray[" << i << "] == hero! " << std::endl;
		}
		else if (vobIsInTouchList(npc, currentVob))
		{
			//ss << "vobArray[" << i << "] belongs to hero!" << std::endl;
		}
		 //(bool)((BYTE*)currentVob + 0x104)
		else if (currentVob->bitfield[0] & collDetectionDynamic)
		{
			//ss << "vobArray[" << i << "] has dynamic collision!" << std::endl;
			// zCVisual is base from both, zCMesh and zCModel
			// Npcs will have zCModel as their visual; There are some other types, too! //TODO!!!
			void* visual = currentVob->GetVisual();
			// assume for now that visual is a zCMesh TODO!!!
			void* mesh = visual;

			if (!mesh) continue;

			// get polylist and check intersection!
			//.text:00572AAB                 mov     eax, [edi + 34h] -> edi + 0x34 contains poly count of mesh!
			//int polyCount = *(int*)(((BYTE*)mesh) + 0x34);
			//ss << "polyCount= " << polyCount << std::endl;

			//get poly list
			//.text:00572AA0                 mov     eax, [edi+44h] -> edi + 0x44 contains begin of poly list
			//zCPolygon* polyList = (zCPolygon*)((BYTE*)mesh + 0x44);

			int zCVisualTypeDesc = 0x00895EF8;
			int zCMeshTypeDesc = 0x008A25D8;
			int zCModelTypeDesc = 0x00895F10;

			int zCModelVtableOffset = 0x0083234C;
			int zCMeshVtableOffset = 0x008322BC;
			int zCVisualVtableOffset = 0x00839A04;
			int zCQuadMarkVtableOffset = 0x00832A64;
			int zCProgMeshProtoVtableOffset = 0x008329CC;
			int zCPolyStripVtableOffset = 0x008326B4;
			int zCParticleFXVtableOffset = 0x008325C4;
			int zCMorphMeshVtableOffset = 0x008324D4;
			int zCDecalVtableOffset = 0x00832084;


			if (*((int*)mesh) == zCMeshVtableOffset)
			{
				//ss << "Visual is a zCMesh!" << std::endl;
			}

			if (*((int*)mesh) == zCVisualVtableOffset)
			{
				//ss << "Visual is a zCVisual!" << std::endl;
			}

			if (*((int*)mesh) == zCModelVtableOffset)
			{
				//ss << "Visual is a zCModel!" << std::endl;
				//.text:005794B3                 mov     eax, [esi + 84h]
				int* securityTest = ((int*)mesh) + 0x84;
				if ((*securityTest) <= 0)
				{
					//ss << "pointerProto is  <= 0!"  << std::endl;
					//util::logAlways(&ss);
					//collided = true;
					//break;
				}
				//ss << "*securityTest= " << *securityTest << std::endl;
				/*BYTE** meshProtoList = ((BYTE**)mesh + 0x7C);
				BYTE* meshProto = (BYTE*)mesh + 0x7C;
				ss << "meshProto= " << meshProto << std::endl;
				int numTris = constants::zCProgMeshProtoGetNumTri(meshProto);
				ss << "Num of Tris: " << numTris << std::endl;
				util::logAlways(&ss);
				for (int i = 0; i < numTris; ++i)
				{
					zCPolygon* poly = (zCPolygon*)constants::zCProgMeshProtoGetPolygon(meshProto, i);
					ss << "poly[" << i << "].GetNumPolyVertices()" << (int)poly->GetNumPolyVertices() << std::endl;
				}*/
			}

			if (*((int*)mesh) == zCQuadMarkVtableOffset)
			{
				//ss << "Visual is a zCQuadMark!" << std::endl;
			}

			if (*((int*)mesh) == zCProgMeshProtoVtableOffset)
			{

				//ss << "Visual is a zCProgMeshProto!" << std::endl;
				int numTris = constants::zCProgMeshProtoGetNumTri(mesh);
				//ss << "Num of Tris: " << numTris << std::endl;

				/*for (int i = 0; i < numTris; ++i)
				{
					zCPolygon* poly = (zCPolygon*)constants::zCProgMeshProtoGetPolygon(mesh, i);
					//ss << "poly[" << i << "].GetNumPolyVertices()" << (int)poly->GetNumPolyVertices() << std::endl;
				}*/

				zVEC3 currentVobPos; currentVob->GetPositionWorld(currentVobPos.x, currentVobPos.y, currentVobPos.z);
				zTBBox3D bbox = box;
				/*bbox.bbox3D_mins.x -= currentVobPos.x;
				bbox.bbox3D_mins.y -= currentVobPos.y;
				bbox.bbox3D_mins.z -= currentVobPos.z;

				bbox.bbox3D_maxs.x -= currentVobPos.x;
				bbox.bbox3D_maxs.y -= currentVobPos.y;
				bbox.bbox3D_maxs.z -= currentVobPos.z;*/
				if (checkIntersection_zCProgMeshProto(mesh, currentVob->trafoObjToWorld, bbox))
				{
					collided = true;
					//ss << "collision detected! " << std::endl;
				}

				//no collision:
				//zVEC3 vobPos; npc->GetPositionWorld(vobPos.x, vobPos.y, vobPos.z);
				//ss << "currentVob position: " << zVEC3ToString(currentVobPos) << std::endl;
				//ss << "mat position: " << zVEC3ToString(pos) << std::endl;
				//ss << "npc position: " << zVEC3ToString(vobPos) << std::endl;
				//util::logAlways(&ss);
			}

			if (*((int*)mesh) == zCPolyStripVtableOffset)
			{
				//ss << "Visual is a zCPolyStrip!" << std::endl;
			}

			if (*((int*)mesh) == zCParticleFXVtableOffset)
			{
				//ss << "Visual is a zCParticleFX!" << std::endl;
			}

			if (*((int*)mesh) == zCMorphMeshVtableOffset)
			{
				//ss << "Visual is a zCMorphMesh!" << std::endl;
			}

			if (*((int*)mesh) == zCDecalVtableOffset)
			{
				//ss << "Visual is a zCDecal!" << std::endl;
			}

			//intersected = true;
			//break;
		}
		else
		{
			//ss << "vobArray[" << i << "] = " << currentVob << std::endl;
		}
	}

	util::logAlways(&ss);

	return collided;
};

bool CollisionDetector::checkIntersection_zCProgMeshProto(void* progMesh, zMAT4 const & mat, const zTBBox3D& boundingBox)
{
	zCProgMeshProto* proto = (zCProgMeshProto*)progMesh;
	zCArrayAdapt<zVEC3>* positions = proto->GetPositionList();
	int numTris = constants::zCProgMeshProtoGetNumTri(progMesh);
	

	zSTRING& name = zCProgMeshProtoGetVisualName(progMesh);
	std::stringstream ss;
	ss << name.ToChar() << std::endl;
	util::logAlways(&ss);

	if (positions == nullptr) return false;

	int size = positions->NumInArray;
	for (int i = 0; i < size; ++i)
	{
		zVEC3 vertex = positions->Get(i);
		//ss << "vertex[" << i << "] = " << vertex << std::endl;
		//util::logAlways(&ss);
	}


	// static for avoiding reallocations
	static  zCVertex polyBuffervertices [3];
	static zCPolygon polyBuffer;

	int numSubMeshes = proto->GetNumSubmeshes();

	//ss << "numSubMeshes: " << numSubMeshes << std::endl;
	//util::logAlways(&ss);

	for (int i = 0; i < numSubMeshes; ++i)
	{
		zCSubMesh* subMesh = proto->GetSubmesh(i);
		//ss << "index start: " << subMesh->vbStartIndex << std::endl;
		//util::logAlways(&ss);
		if (subMesh->Material)
		{
			if (!subMesh->Material->hasNoCollision())
			{
				//ss << "submesh " << i << " has no collision!"<< std::endl;
				//util::logAlways(&ss);
				continue;
			}
		}


		for (int j = 0; j < subMesh->TriList.NumInArray; ++j)
		{
			bool check = true;
			zTPMTriangle triangle = subMesh->TriList.Get(j);
			//ss << "triangle: (" << triangle.edge[0] << ", " << triangle.edge[1] << "," << triangle.edge[2] << ")"<< std::endl;
			//util::logAlways(&ss);
			int index0 = subMesh->WedgeList.Get(triangle.edge[0]).position;
			int index1 = subMesh->WedgeList.Get(triangle.edge[1]).position;
			int index2 = subMesh->WedgeList.Get(triangle.edge[2]).position;

			//ss << "Index list: (" << index0 << ", " << index1 << "," << index2 << ")" << std::endl;
			//util::logAlways(&ss);
			//fill vertex buffer
			polyBuffervertices[0].Position = positions->Get(index0);
			polyBuffervertices[1].Position = positions->Get(index1);
			polyBuffervertices[2].Position = positions->Get(index2);

			transform(mat, polyBuffervertices[0].Position);
			transform(mat, polyBuffervertices[1].Position);
			transform(mat, polyBuffervertices[2].Position);

			//polyBuffervertices[1].Position = constants::zMAT4Rotate(&mat, positions->Get(index1));
			
			//polyBuffervertices[2].Position = constants::zMAT4Rotate(&mat, positions->Get(index2));

			polyBuffer.SetVertices(polyBuffervertices, polyBuffervertices + 1, polyBuffervertices + 2);	

			// test polyBuffer

			for (int k = 0; k < 3; ++k)
			{
				zCVertex* vertex = polyBuffer.getVertices()[k];
				float x = vertex->Position.x;
				float y = vertex->Position.y;
				float z = vertex->Position.z;

				//ss << "poly-vertex[" << k <<"] = " << zVEC3(x, y, z) << std::endl;
				//util::logAlways(&ss);

				if ((isnan(x) || isnan(y) || isnan(z))
					|| (x > 1.0e+10 || y > 1.0e+10 || z > 1.0e+10))
				{
					//we cannot check a poly with undefined vertices
					//ss << "skipping poly..." << std::endl;
					//util::logAlways(&ss);
					check = false;
				}
			}

			if (check && polyBuffer.CheckBBoxPolyIntersection(boundingBox))
			{
				return true;
			}
		}
	}


	/*for (int i = 0; i < numTris; ++i)
	{
		zCPolygon* poly = (zCPolygon*)constants::zCProgMeshProtoGetPolygon(progMesh, i);
		//ss << "poly[" << i << "].GetNumPolyVertices()" << (int)poly->GetNumPolyVertices() << std::endl;
		bool check = true;
		for (int i = 0; i < 3; ++i)
		{
			zCVertex* vertex = poly->getVertices()[i];
			float x = vertex->Position.x;
			float y = vertex->Position.y;
			float z = vertex->Position.z;

			ss << "poly-vertex: " << zVEC3(x, y, z) << std::endl;
			util::logAlways(&ss);

			if ((isnan(x) || isnan(y) || isnan(z))
				|| (x > 1.0e+10 || y > 1.0e+10 || z > 1.0e+10))
			{
				//we cannot check a poly with undefined vertices
				ss << "skipping poly..." << std::endl;
				util::logAlways(&ss);
				check = false;
			}

			//ss << "vertex pos: " << zVEC3ToString(zVEC3(x, y, z)) << std::endl;
			//ss << "Bounding box mins: " << zVEC3ToString(boundingBox.bbox3D_mins) << std::endl;
			//ss << "Bounding box maxs: " << zVEC3ToString(boundingBox.bbox3D_maxs) << std::endl;
			//util::logAlways(&ss);
		}
		if (check && poly->CheckBBoxPolyIntersection(boundingBox))
		{
			//return true;
		}
	}*/

	return false;
}

zMAT4 CollisionDetector::inverse(zMAT4 const & m)
{
	float d = det(m);
	return (1/d) * transpose(getCofactorMatrix(m));
}

zMAT4 CollisionDetector::transpose(zMAT4 const & m)
{
	return zMAT4(m._11, m._21, m._31, m._41, 
		m._12, m._22, m._32, m._42, 
		m._13, m._23, m._33, m._43,
		m._14, m._24, m._34, m._44);
}

float CollisionDetector::det(zMAT4 const & m)
{
	//zMAT3 a3(m._22, m._23, m._24, 0, m._32, m._33, m._34, 0, m._42, m._43, m._44, 0);
	zMAT3 a3 = getMatrixMinor(m , 0, 0);
	//zMAT3 b3(m._21, m._23, m._24, 0, m._31, m._33, m._34, 0, m._41, m._43, m._44, 0);
	zMAT3 b3 = getMatrixMinor(m, 0, 1);
	//zMAT3 c3(m._21, m._22, m._24, 0, m._31, m._32, m._34, 0, m._41, m._42, m._44, 0);
	zMAT3 c3 = getMatrixMinor(m, 0, 2);
	//zMAT3 d3(m._21, m._22, m._23, 0, m._31, m._32, m._33, 0, m._41, m._42, m._43, 0);
	zMAT3 d3 = getMatrixMinor(m, 0, 3);
	return m._11 * det(a3) - m._12 * det(b3) + m._13 * det(c3) - m._14 * det(d3);
}

float CollisionDetector::det(zMAT3 const & m)
{
	return m._11*m._22*m._33 + m._12*m._23*m._31 + m._13*m._21*m._32 -
		m._13*m._22*m._31 - m._12*m._21*m._33 - m._11*m._23*m._32;
}

zMAT3 CollisionDetector::getMatrixMinor(zMAT4 const & m, int rowIndex, int columnIndex)
{
	zMAT3 result (0,0,0,0,0,0,0,0,0,0,0,0);
	int resultI = 0;

	for (int i = 0; i < 4; ++i)
	{
		if (i == rowIndex) continue;
		int resultJ = 0;

		for (int j = 0; j < 4; ++j)
		{
			if (j == columnIndex) continue;
			result.m[resultI][resultJ] = m.m[i][j];
			++resultJ;
		}

		++resultI;
	}

	return result;
}

zMAT4 CollisionDetector::getCofactorMatrix(zMAT4 const & mat)
{
	zMAT4 result;

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			int d = det(getMatrixMinor(mat, i, j));
			int sign = (i + 1) + (j + 1);
			if (sign % 2)
			{
				result.m[i][j] = -d;
			} else
			{
				result.m[i][j] = d;
			}
		}
	}

	return result;
}

bool CollisionDetector::vobIsInTouchList(zCVob* source, zCVob* vobToCheck)
{
	int size = source->touchVobList.GetSize();
	for (int i = 0; i < source->touchVobList.GetSize(); ++i)
	{
		zCVob* vob = source->touchVobList.GetItem(i);
		if (vob == vobToCheck)
		{
			return true;
		}
	}

	return false;
}

zVEC3  _cdecl CollisionDetector::getCollidingPolyNormal(oCNpc* npc, const zMAT4& mat)
{
	bool intersected = false;
	zVEC3 pos(mat.m[0][3], mat.m[1][3], mat.m[2][3]);
	zCPolygon** polys;
	void* pointer = (void*)*(int*)((BYTE*)npc + 0x0b8);
	pointer = (void*)*(int*)((BYTE*)pointer + 0x1b4);
	int polyCount = 0;

	zCModel* model = npc->GetModel();
	constants::zCModelCalcModelBBox3DWorld(model);
	zTBBox3D box = constants::zCModelGetBBox3D(model);

	//make rec a little bit bigger than box
	zTBBox3D rec;
	rec.bbox3D_maxs = zVEC3(100, 100, 100);//box.bbox3D_maxs;//zVEC3(100,100,100);
										   //logStream << "Test:  box.bbox3D_maxs: " << box.bbox3D_maxs << std::endl;
										   //util::logInfo(&logStream);
	rec.bbox3D_mins = zVEC3(-100, -100, -100);// box.bbox3D_mins;//zVEC3(-100, -100, -100);
											  //logStream << "Test:  box.bbox3D_mins: " << box.bbox3D_mins << std::endl;
											  //util::logInfo(&logStream);
	rec.bbox3D_maxs += pos;
	rec.bbox3D_mins += pos;
	box.bbox3D_maxs += pos;
	box.bbox3D_mins += pos;

	constants::zCBspBaseCollectPolysInBBox3D(pointer, rec, polys, polyCount);
	if (polys != nullptr) {
		//writeToConsole("Collision detected: ", third);
		//logStream << "Test: Collision detected: " << third << std::endl;
		//util::logInfo(&logStream);
		if (polyCount)
		{
			//create plane from intersecting poly
			/*Plane plane;
			zVEC3 look = zVEC3(mat.m[0][2], mat.m[1][2], mat.m[2][2]);
			constants::zVEC3Normalize(&look);
			zVEC3 oldPos = zVEC3(heroLevitationBean->oldXPos, heroLevitationBean->oldYPos, heroLevitationBean->oldZPos);
			zVEC3 dir = pos - oldPos;
			constants::zVEC3Normalize(&dir);

			Plane::initPlaneByVectors(plane, pos, look);
			plane.convertToHNF();
			Line motionLine(oldPos, look);*/

			for (int i = 0; i < polyCount; ++i)
			{
				zCPolygon* poly = polys[i];

				if (poly->CheckBBoxPolyIntersection(rec))
				{
					if (poly->CheckBBoxPolyIntersection(box))
					{
						zVEC3* polyNormal = poly->getPolyNormal();

						//we have found what we wanted -> a collision!
						return zVEC3(polyNormal->x, polyNormal->y, polyNormal->z);

					}
				}
			}
		}
	}

	//nothing found - return null vector
	return zVEC3(0, 0, 0);
}

void adaptBounding(zVEC3 const & vec, zTBBox3D* box)
{
	zVEC3* min = &box->bbox3D_mins;
	zVEC3* max = &box->bbox3D_maxs;

	min->x = min(min->x, vec.x);
	min->y = min(min->y, vec.y);
	min->z = min(min->z, vec.z);

	max->x = max(max->x, vec.x);
	max->y = max(max->y, vec.y);
	max->z = max(max->z, vec.z);

}

zTBBox3D getBoundingBox(std::vector<zVEC3*> const & vertices)
{
	zTBBox3D result;
	if (!vertices.size())
	{
		return zTBBox3D{ zVEC3(0,0,0), zVEC3(0,0,0) };
	}

	// init
	result.bbox3D_mins = *vertices[0];
	result.bbox3D_maxs = result.bbox3D_mins;

	for (int i = 1; i < vertices.size(); ++i)
	{
		adaptBounding(*vertices[i], &result);
	}

	return result;
}

zTBBox3D getBoundingBox(zCProgMeshProto* mesh)
{
	zCArrayAdapt<zVEC3>* positions = mesh->GetPositionList();
	zTBBox3D result;
	if (!positions || !positions->NumInArray)
	{
		return zTBBox3D{ zVEC3(0,0,0), zVEC3(0,0,0) };
	}

	// init
	result.bbox3D_mins = positions->Get(0);
	result.bbox3D_maxs = result.bbox3D_mins;

	for (int i = 1; i < positions->NumInArray; ++i)
	{
		adaptBounding(positions->Get(i), &result);
	}

	return result;
};

void printStatisticsVisual(zCVisual* visual)
{
	std::stringstream ss;
	//ss << "printStatisticsVisual(zCVisual*):" << std::endl;
	if (!visual) {
		//ss << "visual is null" << std::endl;
		//util::logAlways(&ss);
		return;
	}
	//ss << "node proto visual type" << visual->stringOfEVisualType(visual->GetVisualType()) << std::endl;
	//util::logAlways(&ss);
};

void printStatisticsOfNode(zCModelNode* node)
{
	std::stringstream ss;
	//ss << "printStatisticsOfNode(zCModelNode*): " << std::endl;
	//ss << "node name: " << node->NodeName.ToChar() << std::endl;
	//ss << "node visual: " << node->Visual << std::endl;
	printStatisticsVisual(node->Visual);
	//util::logAlways(&ss);
}

void printStatisticsOfModelProto(zCModelPrototype* proto)
{
	std::stringstream ss;

	//ss << "printStatisticsOfModelProto: " << std::endl;
	//ss << "visual name: " << proto->GetVisualName() << std::endl;
	zCArray<zCModelNode*>* nodes = proto->GetNodeList();

	if (nodes)
	{
		for (int i = 0; i < nodes->GetSize(); ++i)
		{
			printStatisticsOfNode(nodes->GetItem(i));
		}
	}

	//util::logAlways(&ss);
};

std::ostream& operator<< (std::ostream& os, zTWeightEntry& entry) {
	os << "(" << entry.Weight << ", " << entry.VertexPosition << ", " << (int)entry.NodeIndex;
	return os;
}

void printStatisticsSkin(zCMeshSoftSkin* skin, std::vector<std::vector<zVEC3*>>& boneVertList)
{
	if (!skin) return;
	std::stringstream ss;

	//ss << "printStatisticsSkin(): " << std::endl;
	//ss << "num sub meshes: " << skin->GetNumSubmeshes() << std::endl;
	//ss << "object name:" << skin->GetObjectName() << std::endl;
	// skins are usually zCProgMeshProto
	//ss << "visual type: " << skin->stringOfEVisualType(skin->GetVisualType()) << std::endl;
	zTBBox3D boundingBox = getBoundingBox(skin);

	//ss << "bounding box mins = " << boundingBox.bbox3D_mins << std::endl;
	//ss << "bounding box maxs = " << boundingBox.bbox3D_maxs << std::endl;


	char* stream = skin->GetVertWeightStream();

	for (int i = 0; i < skin->GetPositionList()->NumInArray; ++i)
	{
		//ss << "vertexWeight[" << i << "].NodeIndex = " << *(skin->GetVertWeight(i)) << std::endl;
		int numOfNodes = *stream;
		stream += 4;
		//ss << i << ": num of nodes: " << numOfNodes << std::endl;
		//ss << "vertex position: " << skin->GetPositionList()->Get(i) << std::endl;
		for (int j = 0; j < numOfNodes; ++j)
		{
			zTWeightEntry* entry = (zTWeightEntry*)stream;
			stream += sizeof(zTWeightEntry);

			//collect for all bones vertices that are weigthed to them
			boneVertList[(int)entry->NodeIndex -1].push_back(&(skin->GetPositionList()->Array[i]));
		}
	}
	//util::logAlways(&ss);
}

void printStatisticsOfNodeInst(zCModelNodeInst* node)
{
	if (!node) return;
	std::stringstream ss;
	zCModelNode* proto = node->ProtoNode;

	//ss << "node " << ": " << std::endl;
	printStatisticsVisual(node->NodeVisual);
	//ss << "node parent: " << node->ParentNode << std::endl;
	//ss << "node proto: " << proto << std::endl;
	//ss << "node bounding box mins: " << node->BBox3D.bbox3D_mins << std::endl;
	//ss << "node bounding box maxs: " << node->BBox3D.bbox3D_maxs << std::endl;
	printStatisticsOfNode(proto);
	//util::logAlways(&ss);
}

void printStatisticszTNodeMesh(zCModelMeshLib::zTNodeMesh const& nodeMesh)
{
	std::stringstream ss;
	//ss << "printStatisticszTNodeMesh(zCModelMeshLib::zTNodeMesh&):" << std::endl;

	//ss << "visual = " << nodeMesh.Visual << std::endl;
	//ss << "nodeIndex = " << nodeMesh.NodeIndex << std::endl;

	//util::logAlways(&ss);
};

void printStatisticsMeshLib(zTMeshLibEntry* meshLib, std::vector<std::vector<zVEC3*>>& boneVertList)
{
	std::stringstream ss;
	//ss << "printStatisticsMeshLib(zTMeshLibEntry*):" << std::endl;
	if (!meshLib)
	{
		//ss << "meshLib is null!" << std::endl;
		//util::logAlways(&ss);
		return;
	}
	zCModelMeshLib* modelMeshLib = meshLib->MeshLibPtr;
	ss << modelMeshLib << std::endl;

	if (!modelMeshLib)
	{
		util::logAlways(&ss);
		return;
	}

	//ss << "modelMeshLib visual name: " << modelMeshLib->GetVisualName() << std::endl;
	zCArray<zCMeshSoftSkin*>* skins = modelMeshLib->GetMeshSoftSkinList();
	if (!skins)
	{
		util::logAlways(&ss);
		return;
	}

	for (int i = 0; i < skins->GetSize(); ++i)
	{
		printStatisticsSkin(skins->GetItem(i), boneVertList);
	}

	zCArray<zCModelMeshLib::zTNodeMesh>* entries = modelMeshLib->GetNodeList();
	//ss << "entries: " << entries << std::endl;
	if (!entries)
	{
		//util::logAlways(&ss);
		return;
	}

	for (int i = 0; i < entries->GetSize(); ++i)
	{
		printStatisticszTNodeMesh(entries->GetItem(i));
	}

	//util::logAlways(&ss);
};

//.text:0053A170 void __cdecl DrawBox(struct zTBBox3D const &) proc near
typedef void(__cdecl* DrawBox)(zTBBox3D const &);
DrawBox drawBox = (DrawBox)0x0053A170;

std::ostream& operator << (std::ostream& stream, const zMAT4 & m) {

	stream << "(" << m._11 << "," << m._12 << ", " << m._13 << ", " << m._14 << std::endl;
	stream << m._21 << "," << m._22 << ", " << m._23 << ", " << m._24 << std::endl;
	stream << m._31 << "," << m._32 << ", " << m._33 << ", " << m._34 << std::endl;
	stream << m._41 << "," << m._42 << ", " << m._43 << ", " << m._44 << ")" << std::endl;
	return stream;
};

std::vector<zTBBox3D>* boundingBoxes;
bool createdBoundingBoxes;

void CollisionDetector::doStatistics(oCNpc* npc)
{
	zCModel* model = npc->GetModel();
	std::stringstream ss;
	zCVisual* npcVisual = npc->GetVisual();
	std::vector<std::vector<zVEC3*>> boneVertList;
	//ss << "doStatistics:" << std::endl;
	if (npcVisual == model)
	{
		//ss << "npcVisual is equal to model" << std::endl;
	}

	//util::logAlways(&ss);

	printStatisticsVisual(npcVisual);

	std::string visual = model->GetVisualName();
	//ss << "doStatistics: model->visual= " << visual << std::endl;
	//util::logAlways(&ss);

	zCArray<zCModelNodeInst*>* nodes = model->GetNodeList();
	zCArray<zCModelPrototype*>* protos = model->GetModelProtoList();
	zCArray<zTMeshLibEntry*>* meshLibs = model->GetMeshLibList();
	int nodeNum;
	if (nodes == nullptr)
	{
		//ss << "doStatistics: no nodes assigned!" << std::endl;
		//util::logAlways(&ss);
		nodeNum = 0;

	} else
	{
		nodeNum = nodes->m_numInArray;
	}

	if (!nodeNum) return;

	//ss << "doStatistics: node number= " << nodeNum << std::endl;
	util::logAlways(&ss);

	for (int i = 0; i < nodeNum; ++i)
	{
		zCModelNodeInst* node = nodes->GetItem(i);
		//printStatisticsOfNodeInst(node);
	}

	if (!protos) return;

	for (int i = 0; i < protos->m_numInArray; ++i)
	{
		//printStatisticsOfModelProto(protos->GetItem(i));
	}

	if (!meshLibs) return;
	for (int i = 0; i < 1; ++i)
	{
		boneVertList.resize(nodeNum);
		printStatisticsMeshLib(meshLibs->GetItem(i), boneVertList);
	}

	zVEC3 heroPos; oCNpc::GetHero()->GetPositionWorld(heroPos.x, heroPos.y, heroPos.z);
	D3DXMATRIX heroMat = *(D3DXMATRIX*)&oCNpc::GetHero()->trafoObjToWorld;
	std::vector<D3DXMATRIX> boneTransforms;
	model->GetBoneWorldTransforms(&boneTransforms);

	for (int i = 0; i < boneVertList.size(); ++i)
	{
		zCModelNodeInst* node = model->GetNodeList()->m_array[i];
		
		if (!createdBoundingBoxes) {
			if (!boundingBoxes) {
				boundingBoxes = new std::vector<zTBBox3D>();
				boundingBoxes->resize(boneVertList.size());
			}
			(*boundingBoxes)[i] = getBoundingBox(boneVertList[i]);

		}

		zTBBox3D boundingBox = (*boundingBoxes)[i];

		//boundingBox.bbox3D_maxs -= boundingBox.bbox3D_mins;
		//boundingBox.bbox3D_mins = zVEC3(0,0,0);

		zCModelNodeInst* parent = node->ParentNode; 
		D3DXMATRIX trans; D3DXMatrixIdentity(&trans);
		Box::setTranslation(&trans, zVEC3(0, 0, 0));
		D3DXMATRIX temp;
		zVEC3 scale = model->GetModelScale();
		while(parent)
		{
			temp = parent->Trafo;
			trans = temp* trans;
			parent = parent->ParentNode;
		}

		BYTE* camera = (BYTE*)0x008D7F94;

		D3DXMATRIX cameraTrafo = *(D3DXMATRIX*)(camera + 164);
		//trans = cameraTrafo * trans;

		boundingBox.bbox3D_mins = zVEC3(0,0,0);
		boundingBox.bbox3D_maxs = zVEC3(25, 25, 25);
		Box box(boundingBox, node, zCOLOR(255,255,0));
		box.setTranslation(zVEC3(0, 0, 0));
		D3DXMATRIX matBox = box.getTransformation();

		matBox =  heroMat * boneTransforms[i] *  matBox;
		if (node->ParentNode)
		{
			D3DXMATRIX parentMat = trans;
			D3DXMATRIX parentTrafo = heroMat * parentMat;
			box.setParentTrans(box.getTranslation(parentTrafo));
			D3DXMATRIX heroRot = heroMat;
			trans = node->ParentNode->Trafo;
			Box::setTranslation(&trans, zVEC3(0, 0, 0));
			D3DXMatrixInverse(&heroRot, NULL, &heroRot);
			box.setParentTrafo(trans * heroRot);

		} else
		{
			box.setHasParent(false);
		}

		box.setTransformation(matBox);

		D3DXMATRIX heroRot = heroMat;
		trans = node->Trafo;
		Box::setTranslation(&trans, zVEC3(0, 0, 0));
		D3DXMatrixInverse(&heroRot, NULL, &heroRot);
		box.setOrientation(heroRot);
		box.setRotation(trans);
		
		if (i != 0)
		{
			box.drawBox = false;
		}

		zTBBox3D bboxHero = constants::zCModelGetBBox3D(oCNpc::GetHero()->GetModel());
		box.bbox.local00.y = bboxHero.bbox3D_mins.y;
		box.bbox.local01.y = bboxHero.bbox3D_mins.y;
		box.bbox.local10.y = bboxHero.bbox3D_mins.y;
		box.bbox.local11.y = bboxHero.bbox3D_mins.y;

		box.bbox.parent00.y = bboxHero.bbox3D_maxs.y;
		box.bbox.parent01.y = bboxHero.bbox3D_maxs.y;
		box.bbox.parent10.y = bboxHero.bbox3D_maxs.y;
		box.bbox.parent11.y = bboxHero.bbox3D_maxs.y;
		box.draw();

		//constants::drawLine(heroPos, heroPos + zVEC3(0,40,0), zCOLOR(0,255,0), 0);
		//constants::drawLine(heroPos, heroPos + zVEC3(40, 0, 0), zCOLOR(255, 0, 0), 0);
		//constants::drawLine(heroPos, heroPos + zVEC3(0, 0, 40), zCOLOR(0, 0, 255), 0);
	}
	createdBoundingBoxes = true;

	//CollisionObjectTest
	//load mesh
	zCVisual* mesh = zCProgMeshProto::Load(zSTRING("COLLISIONOBJECTTEST.3ds"));
	//ss << "mesh: " << mesh << std::endl;

	if (mesh)
	{
		/*ss << "mesh name: " << mesh->GetObjectName() << std::endl;
		ss << "mesh type: " << mesh->stringOfEVisualType(mesh->GetVisualType()) << std::endl;
		ss << "mesh file extension: " << mesh->GetFileExtension(0) << std::endl;*/

		int zCProgMeshProtoVtableOffset = 0x008329CC;
		if (*(int*)mesh == zCProgMeshProtoVtableOffset)
		{
			//ss << "mesh is a zCProgMeshProto!" << std::endl;
			zCProgMeshProto* proto = (zCProgMeshProto*)mesh;
			//ss << "sub meshes num: " << proto->GetNumSubmeshes()<< std::endl;

			for (int i = 0; i < proto->GetNumSubmeshes(); ++i)
			{
				zCSubMesh* subMesh = proto->GetSubmesh(i);
				//ss << "sub meshes material name: " << subMesh->Material->GetObjectName().ToChar() << std::endl;
			}
		}
	}
	//ss << "mesh name: " << mesh->GetObjectName() << std::endl;
	//ss << "mesh type: " << mesh->stringOfEVisualType(mesh->GetVisualType()) << std::endl;
	util::logAlways(&ss);

}
//.text:00738480 public: void __thiscall oCNpc::InitModel(void) proc near
typedef void(__thiscall* OCNpcInitModel)(oCNpc*);
OCNpcInitModel oCNpcInitModel = (OCNpcInitModel)0x00738480;

bool CollisionDetector::detectCollision(oCNpc* npc, zMAT4& mat)
{
	doStatistics(npc);

	bool collided = false;//detectStaticCollision(npc, mat);
	if (!collided)
	{
		//collided = detectDynamicCollision(npc, mat);
		//if (!collided)
		   //collided = npc->DetectCollision(&mat);
	}

	return collided;
}