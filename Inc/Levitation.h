#ifndef __LEVITATION_H
#define __LEVITATION_H

#include <Module.h>
#include <api/g2/ztypes.h>
#include "api/g2/ocnpc.h"

#define ZCVOB_DO_FRAME_ACTIVITY_ADDRESS 0x00602C60
#define ZCVOB_SET_PHYSICS_ENABLED_ADDRESS 0x0061D190
#define ZCVOB_CHECK_AUTO_UNLINK_ADDRESS 0x0061AAC0
#define OCGAME_PAUSE_ADDRESS 0x006C8AD0
#define OCGAME_UNPAUSE_ADDRESS 0x006C8D50
#define ZCVOB_SET_COLL_DET_ADDRESS 0x006D0000		
#define ZCAIPLAYER_CHECK_FLOOR_SLIDING_ADDRESS 0x0050D4D0
#define DO_SURFACE_ALIGNMENT_ADDRESS 0x0050CE80 //0x0050D4D0//Original: 0x0050CE80
#define ZCVOB_CHECK_AND_RESOLVE_COLLISION_ADDRESS  0x0061F060//0x0061D190//0x0061E0D0 //Test: 0x0061F060
#define ZCTRIGGER_ON_TOUCH_ADDRESS 0x00610640
#define ZCVOB_HAS_ENOUGH_SPACE_ADDRESS 0x0077C6B0
#define ZCVOB_GET_VELOCITY_ADDRESS 0x0061D1C0
#define ZCBSPBASE_COLLECT_POLYS_IN_BBOX3D 0x00532EB0
#define ZCVOB_IS_COLLIDING 0x0061F4F0


#define THISPTR_OFFSET(x) (((char *)this) + (x))

class Float
{
public:
	static const float EPSILON;
	static bool areEqual(const float& a, const float& b)
	{
		if (abs(a - b) < EPSILON)
		{
			return true;
		}
		return false;
	};

};

namespace LevitationUtil
{
	//.text:00490EA0 public: class zVEC3 & __thiscall zVEC3::Normalize(void) proc near
	typedef int(__thiscall* ZVEC3Normalize)(zVEC3*); 
	extern ZVEC3Normalize zVEC3Normalize;

	//.text:006824D0 public: virtual void __thiscall oCNpc::ResetPos(class zVEC3 &) proc near
	typedef void(__thiscall* OCNpcResetPos)(oCNpc*, zVEC3 &);
	extern OCNpcResetPos oCNpcResetPos;

	//.text:0061E0D0 public: virtual void __thiscall zCVob::EndMovement(int) proc near
	typedef void(__thiscall* ZCVobEndMovement)(zCVob*, int);
	extern ZCVobEndMovement zCVobEndMovement;

	inline float getDistanceOfVectors(const zVEC3 & vec1, const zVEC3 & vec2) {
		float distance = abs(vec2.x - vec1.x + vec2.y - vec1.y + vec2.z - vec1.z);
		if (Float::areEqual(distance, 0)) {
			return 0;
		}
		return distance;
	}

	inline bool zVEC3IsNull(const zVEC3 & vec)
	{
		if (Float::areEqual(vec.x, 0)
			&& Float::areEqual(vec.y, 0)
			&& Float::areEqual(vec.z, 0))
		{
			return true;
		}
		return false;
	}

};

struct GothicMemoryLocations
{
	struct zCPolygon
	{
		static const unsigned int Offset_VerticesArray = 0x00;
		static const unsigned int Offset_PolyNormalX = 0x0C;
		static const unsigned int Offset_PolyNormalY = 0x10;
		static const unsigned int Offset_PolyNormalZ = 0x14;
		static const unsigned int Offset_FeaturesArray = 0x2C;
		static const unsigned int Offset_NumPolyVertices = 0x30;
		static const unsigned int Offset_PolyFlags = 0x31;
		static const unsigned int Offset_Material = 0x18;
		static const unsigned int Offset_Lightmap = 0x1C;
	};

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

	};
};

const int zMAT_GROUP_WATER = 5;
const int zMAT_GROUP_SNOW = 6;
class zCMaterial
{
public:
	int GetMatGroup()
	{
		return *(int *)((char*)this) + (GothicMemoryLocations::zCMaterial::Offset_MatGroup);
	}

	static int __thiscall IsPortalMaterial(void* pThis);
};



/*  From Degenerated! (TODO: Ask him!)*/
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

struct zCVertex
{
public:
	/*#ifdef BUILD_GOTHIC_1_08k
	int id;
	#endif*/

	float Position[3];

	int TransformedIndex;
	int MyIndex;
};

class zCVertFeature
{
public:
	float normal[3];
	DWORD lightStatic;
	DWORD lightDynamic;
	float texCoord[2];
};




class zCPolygon
{
private:
	char data[56];
public:
	zCVertex** getVertices() const
	{
		return *(zCVertex ***)THISPTR_OFFSET(GothicMemoryLocations::zCPolygon::Offset_VerticesArray);
	}

	zVEC3* getPolyNormal() const
	{
		return (zVEC3 *)THISPTR_OFFSET(GothicMemoryLocations::zCPolygon::Offset_PolyNormalX);
	}

	zCMaterial* GetMaterial() const
	{
		return *(zCMaterial **) THISPTR_OFFSET(GothicMemoryLocations::zCPolygon::Offset_Material);
	}

	unsigned char GetNumPolyVertices() const
	{
		return *(unsigned char*)THISPTR_OFFSET(GothicMemoryLocations::zCPolygon::Offset_NumPolyVertices);
	}

	//.text:005BBA70 public: int __fastcall zCPolygon::CheckBBoxPolyIntersection(struct zTBBox3D const &)const proc near
	int __fastcall CheckBBoxPolyIntersection(zTBBox3D const &) const
	{
		XCALL(0x005BBA70);
	}

	//.text:005B8180 public: int __thiscall zCPolygon::CalcNormal(void) proc near
	int __thiscall CalcNormal()
	{
		XCALL(0x005B8180);
	}
};


class Line
{
public:
	zVEC3 pos;
	zVEC3 dir;
	
	Line(zVEC3 pos, zVEC3 dir)
	{
		this->pos = zVEC3(pos.x, pos.y, pos.z);
		this->dir = zVEC3(dir.x, dir.y, dir.z);
	};

	zVEC3 getPointByLambda(float lambda) const
	{
		float x = pos.x + lambda*dir.x;
		float y = pos.y + lambda*dir.y;
		float z = pos.z + lambda*dir.z;
		return zVEC3(x, y, z);
	}
};

/*
 * A plane described in coordinate form: ax + by + cz = d
 */
class Plane
{
public:
	float a;
	float b;
	float c;
	float d;

	static void initPlaneByVectors(Plane& plane, zVEC3 pos, zVEC3 normal)
	{
		LevitationUtil::zVEC3Normalize(&normal);
		plane.a = normal.x;
		plane.b = normal.y;
		plane.c = normal.z;
		plane.d =(pos.x*normal.x + pos.y*normal.y + pos.z*normal.z);
	};

	float getLengthOfNormalVec() const
	{
		float length = sqrtf(a*a + b*b + c*c);
		// check if 
		if (Float::areEqual(length, 1))
		{
			return 1;
		} 
		
		if (Float::areEqual(length, 0))
		{
			return 0;
		}

		return length;
	};

	/*
	 * Brings this plane to Hesse normal form (HNF)
	 */
	void convertToHNF()
	{
		float length = getLengthOfNormalVec();
		// check if 
		if (!Float::areEqual(length, 1))
		{
			a /= length;
			b /= length;
			c /= length;
			d /= length;
		}
	}
	 
	/*
	 * Returns the minimal directed distane from the given point to this plane.
	 * Directed means in this context whether the point lies in front the plane 
	 * or behind it. A returned positive number means that the point lies in front
	 * the plane whereas a negative number indicates a position behind the plane.
	 * If zero is returned, the point lies onto the plane.
	 */
	float getDirectedDistanceToPlane(const zVEC3 & point) const
	{
		float normalLength = getLengthOfNormalVec();
		float result = (a *point.x + b*point.y + c*point.z - d) / normalLength;
		if (Float::areEqual(result, 0)) {
			result = 0;
		}
		return result;
	}

	/*
	 * Calculates scalar point product and returns false if the result is zero.
	 */
	bool isLineIntersecting(const Line& line) const
	{
		zVEC3 dir = line.dir;
		float scalar = a*dir.x + b*dir.y + c*dir.z;
		return !Float::areEqual(scalar, 0);
	}

	float getLambdaIntersectionValueForLine(const Line& line) const
	{
		float lambda;
		zVEC3 dir = line.dir;
		float x = a * line.pos.x;
		float y = b * line.pos.y;
		float z = c * line.pos.z;
		float lambdaCount = a * dir.x + b * dir.y + c * dir.z;

		if (!Float::areEqual(lambdaCount, 0))
		{
			lambda = (d - x - y - z) / lambdaCount;
		} else
		{
			// lambda == 0 means there are no or infinite intersections
			// or the line position vector is intersecting this plane!
			lambda = 0;
		}

		return lambda;
	}

	/*
	 * Provides the intersection point from this plane and a specified line.
	 * It is assumed, that the line is intersecting this plane.
	 */
	zVEC3 getIntersectionPointToLine(const Line& line) const
	{
		float lambda = getLambdaIntersectionValueForLine(line);
		return line.getPointByLambda(lambda);
	}

	friend std::ostream & operator<<(std::ostream &os, const Plane& p);
};


extern int COLLISION_HEAD_SIZE_Y;

class Levitation : public Module
{
public:

	static bool adjustHeroPosition;
	static bool pausedGame;
	static bool noCollision;
	static int frameTime;
	static int diffFrameTime;
	static float yPos; //not really used!!!

	Levitation();
	virtual ~Levitation();
	virtual void hookModule() override;
	virtual void unHookModule() override;

	static void __thiscall zCVobDoFrameActivityHook(void* pThis);
	static void __thiscall zCVobSetPhysicsEnabledHook(void* pThis, int second);
	static void __thiscall oCGamePauseHook(void* pThis, int second);
	static void __thiscall oCGameUnpauseHook(void* pThis);
	static void __thiscall zCVobSetCollDetHook(void* pThis, int second); //not used!!!
	static int __thiscall zCAIPlayerCheckFloorSlidingHook(void* pThis); //not used!!!
	static void __thiscall DoSurfaceAlignmentHook(void* pThis); //not really used!!!
	static void __thiscall zCVobCheckAndResolveCollisionsHook(void* pThis);
	static void __thiscall zCTriggerOnTouchHook(void* pThis, zCVob*); //not really(?) used!!!
	static int __thiscall zCVobHasEnoughSpaceHook(void* pThis, zVEC3&);  //not really(?) used!!!
	static zVEC3 __thiscall zCVobGetVelocityHook(void* pThis); //not really used!!!

															   //non hooks
	static int __thiscall zCVobIsColliding(void* pThis);
	static int __fastcall zCBspBaseCollectPolysInBBox3D(void*, zTBBox3D const &, zCPolygon** &, int&);

	//.text:00511320 public: int __thiscall zCAIPlayer::CheckEnoughSpaceMoveDir(class zVEC3 const &, int) proc near
	static int __thiscall zCAIPlayerCheckEnoughSpaceMoveDirHook(void* pThis, zVEC3 const &, int);
};

struct Motion
{
	zVEC3 normal;
	float distance;
	zVEC3 objectPos;
	zVEC3 intersect;
};

/*class zCMaterial {
public:
	static int __thiscall IsPortalMaterial(void* pThis);
};*/

__int32 __cdecl sysGetTime();
void levitate();
void Test(oCNpc* npc);
int calcPastFrameTime();
Motion getCollideYDir(zVEC3, float, bool);
Motion getCollideForwardDir(oCNpc*, float, bool);
void __cdecl zCVobCheckAutoUnlinkHook(zCVob* vob);

#endif __LEVITATION_H