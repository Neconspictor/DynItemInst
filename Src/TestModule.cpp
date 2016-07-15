#include "TestModule.h"
#include <HookManager.h>
#include <api/g2/ocgame.h>
#include <api/g2/zcworld.h>
#include <api/g2/ocnpc.h>

//.text:0061D190 public: void __thiscall zCVob::SetPhysicsEnabled(int) proc near
typedef void(__thiscall* ZCVobSetPhysicsEnabled)(zCVob*, int);
ZCVobSetPhysicsEnabled zCVobSetPhysicsEnabled = (ZCVobSetPhysicsEnabled)0x0061D190;

//.text:00602C60 public: void __thiscall zCVob::DoFrameActivity(void) proc near
typedef void(__thiscall* ZCVobDoFrameActivity)(zCVob*);
ZCVobDoFrameActivity zCVobDoFrameActivity = (ZCVobDoFrameActivity)0x00602C60;

//.text:0061DA80 public: void __thiscall zCVob::BeginMovement(void) proc near
typedef void(__thiscall* ZCVobBeginMovement)(zCVob*);
ZCVobBeginMovement zCVobBeginMovement = (ZCVobBeginMovement)0x0061DA80;

//.text:0061B720 public: void __thiscall zCVob::RotateLocalY(float) proc near
typedef void(__thiscall* ZCVobRotateLocalY)(zCVob*, float);
ZCVobRotateLocalY zCVobRotateLocalY = (ZCVobRotateLocalY)0x0061B720;

//.text:0061E0D0 public: virtual void __thiscall zCVob::EndMovement(int) proc near
typedef void(__thiscall* ZCVobEndMovement)(zCVob*, int);
ZCVobEndMovement zCVobEndMovement = (ZCVobEndMovement)0x0061E0D0;

//.text:0061BBD0 public: void __thiscall zCVob::SetTrafo(class zMAT4 const &) proc near
typedef void(__thiscall* ZCVobSetTrafo)(zCVob*, zMAT4 const &);
ZCVobSetTrafo zCVobSetTrafo = (ZCVobSetTrafo)0x0061BBD0;

//.text:0061B830 public: void __thiscall zCVob::RotateWorldY(float) proc near
typedef void(__thiscall* ZCVobRotateWorldY)(zCVob*, float);
ZCVobRotateWorldY zCVobRotateWorldY = (ZCVobRotateWorldY)0x0061B830;

//.text:004B9D90 public: void __thiscall zMAT4::SetUpVector(class zVEC3 const &) proc near
typedef void(__thiscall* ZMAT4SetUpVector)(zMAT4*, zVEC3 const &);
ZMAT4SetUpVector zMAT4SetUpVector = (ZMAT4SetUpVector)0x004B9D90;

//.text:004B9DB0 public: void __thiscall zMAT4::SetRightVector(class zVEC3 const &) proc near
typedef void(__thiscall* ZMAT4SetRightVector)(zMAT4*, zVEC3 const &);
ZMAT4SetRightVector zMAT4SetRightVector = (ZMAT4SetRightVector)0x004B9DB0;

//.text:00431D20 public: class zVEC3 __thiscall zMAT4::GetAtVector(void)const proc near

//.text:0056B960 public: void __thiscall zMAT4::SetAtVector(class zVEC3 const &) proc near
typedef void(__thiscall* ZMAT4SetAtVector)(zMAT4*, zVEC3 const &);
ZMAT4SetAtVector zMAT4SetAtVector = (ZMAT4SetAtVector)0x0056B960;

//.text:00487C50 public: class zMAT4 & __thiscall zMAT4::SetTranslation(class zVEC3 const &) proc near
typedef zMAT4 &(__thiscall* ZMAT4SetTranslation)(zMAT4*, zVEC3 const &);
ZMAT4SetTranslation zMAT4SetTranslation = (ZMAT4SetTranslation)0x00487C50;

//.text:00408EE0 public: class zVEC3 __thiscall zMAT4::GetTranslation(void)const proc near
typedef zVEC3 (__thiscall* ZMAT4GetTranslation)(zMAT4*);
ZMAT4GetTranslation zMAT4GetTranslation = (ZMAT4GetTranslation)0x00408EE0;

//.text:00517730 public: void __thiscall zMAT4::PostRotateX(float) proc near
typedef void(__thiscall* ZMAT4PostRotateX)(zMAT4*, float);
ZMAT4PostRotateX zMAT4PostRotateX = (ZMAT4PostRotateX)0x00517730;

//.text:00517780 public: void __thiscall zMAT4::PostRotateY(float) proc near
typedef void(__thiscall* ZMAT4PostRotateY)(zMAT4*, float);
ZMAT4PostRotateY zMAT4PostRotateY = (ZMAT4PostRotateY)0x00517780;

//.text:0052DCF0 public: class zVEC3 __thiscall zCVob::GetRightVectorWorld(void)const proc near
typedef zVEC3(__thiscall* ZCVobGetRightVectorWorld)(zCVob*);
ZCVobGetRightVectorWorld zCVobGetRightVectorWorld = (ZCVobGetRightVectorWorld)0x0052DCF0;

//.text:0052DCD0 public: class zVEC3 __thiscall zCVob::GetUpVectorWorld(void)const proc near
typedef zVEC3(__thiscall* ZCVobGetUpVectorWorld)(zCVob*);
ZCVobGetUpVectorWorld zCVobGetUpVectorWorld = (ZCVobGetUpVectorWorld)0x0052DCD0;

//.text:00517330 public: void __thiscall zMAT4::MakeOrthonormal(void) proc near
typedef void(__thiscall* ZMAT4MakeOrthonormal)(zMAT4*);
ZMAT4MakeOrthonormal zMAT4MakeOrthonormal = (ZMAT4MakeOrthonormal)0x00517330;

//.text:00517B50 class zMAT4 __cdecl Alg_Identity3D(void) proc near
typedef zMAT4(__cdecl* Alg_Identity3D)();
Alg_Identity3D alg_Identity3D = (Alg_Identity3D)0x00517B50;

std::list<zCVob*> TestModule::vobList;

Vector2D::Vector2D(): x(0), y(0)
{
}

Vector2D::Vector2D(float xPos, float yPos)
{
	x = xPos;
	y = yPos;
}

Vector2D::~Vector2D()
{
}

float Vector2D::squareLength() const
{
	return std::sqrtf(x*x + y*y);
}

float Vector2D::cross(const Vector2D& other) const
{
	return x*other.y - y*other.x;
}

float Vector2D::dot(const Vector2D& other) const
{
	return x* other.x + y* other.y;
}

float Vector2D::angleBetween(const Vector2D& from, const Vector2D& to)
{
	float lengthFrom = from.squareLength();
	float lengthTo = to.squareLength();
	
	if (lengthFrom > 0.0f && lengthTo > 0.0f)
	{
		float sign = from.cross(to);
		float length = lengthFrom * lengthTo;
		if (sign < 0.0f)
		{
			return  -std::acos(from.dot(to) / std::sqrtf(length));
		} else
		{
			return std::acos(from.dot(to) / std::sqrtf(length));
		}
			
	}
	return 0.0f;
}

TestModule::TestModule()
{
	moduleDesc = "TestModule";
}

TestModule::~TestModule()
{
}

void TestModule::hookModule()
{
	HookManager* manager = HookManager::getHookManager();
	manager->addFunctionHook((LPVOID*)&zCVobDoFrameActivity, zCVobDoFrameActivityHook, moduleDesc);
}

void TestModule::unHookModule()
{
	HookManager* manager = HookManager::getHookManager();
	manager->removeFunctionHook((LPVOID*)&zCVobDoFrameActivity, zCVobDoFrameActivityHook, moduleDesc);
}


void createIdentityMatrix(zMAT4* matrix)
{
	zMAT4SetRightVector(matrix, zVEC3(1, 0, 0));
	zMAT4SetUpVector(matrix, zVEC3(0, 1, 0));
	zMAT4SetUpVector(matrix, zVEC3(0, 0, 1));
	*matrix = zMAT4SetTranslation(matrix, zVEC3(0,0,0));
	matrix->_14 = 0;
	matrix->_24 = 0;
	matrix->_34 = 0;
	matrix->_44 = 1;
}

void TestModule::Test()
{
	for (auto it = vobList.begin(); it != vobList.end(); ++it)
	{
		Test(*it);
		//it = vobList.erase(it);
	}
}

bool called = false;
float total = 0;
float factor = 1.0f / 30.0f;
float factor2 = 1.0f / 30.0f;
zMAT4 matrix;

void TestModule::Test(zCVob* vob)
{
/*	
	bool isInList = false;
	auto vobIt = vobList.begin();

	for (auto it = vobList.begin(); it != vobList.end(); ++it)
	{
		if ((*it) == vob)
		{
			vobList.erase(it);
			isInList = true;
			break;
		}
	}

	if (!isInList)
		return;
*/
	//zVEC3 pos; 
	//vob->GetPositionWorld(pos.x, pos.y, pos.z);
	//zMAT4 matrix = alg_Identity3D();

	//zVEC3 at = vob->GetAtVectorWorld();
	//zVEC3 right = zCVobGetRightVectorWorld(vob);
	//zVEC3 up = zCVobGetUpVectorWorld(vob);
	//zMAT4PostRotateX(&matrix, -45);
	//zMAT4PostRotateY(&matrix, 180);

	//zMAT4SetRightVector(&matrix, right);
	//zMAT4SetUpVector(&matrix, up);
	//zMAT4SetAtVector(&matrix, at);

	//zMAT4SetTranslation(&matrix, pos);
	//zMAT4MakeOrthonormal(&matrix);

	//zCVobSetTrafo(vob, matrix);
	//zCWorld* world = oCGame::GetGame()->GetWorld();
	//world->RemoveVob(vob);
	//vob->SetCollDetStat(0);
	//zCVobSetPhysicsEnabled(vob, false);
	zVEC3 pos, heroPos, diff, heroLook;
	vob->GetPositionWorld(pos.x, pos.y, pos.z);
	oCNpc* hero = oCNpc::GetHero();
	hero->GetPositionWorld(heroPos.x, heroPos.y, heroPos.z);
	heroLook = zVEC3(hero->trafoObjToWorld.m[0][2], hero->trafoObjToWorld.m[1][2], hero->trafoObjToWorld.m[2][2]);

	//alg_Identity3D();
	//zMAT4SetTranslation(&matrix, pos);
	//matrix.m[1][4] = pos.x
	//vob->SetTrafoObjToWorld(matrix);

	vob->SetCollDet(false);
	zCVobSetPhysicsEnabled(vob, false);

	zCVobBeginMovement(vob);


	if (!called)
	{
		called = true;
		//matrix = *vob->trafo;
		matrix = vob->trafoObjToWorld;
	}



	if (total >= 1 || total < 0)
	{
		factor *= -1;
	}

	total += factor;

	vob->trafoObjToWorld = matrix;
	//vob->SetPositionWorld(pos);
	vob->trafoObjToWorld.m[0][3] = pos.x;
	vob->trafoObjToWorld.m[1][3] = pos.y;
	vob->trafoObjToWorld.m[2][3] = pos.z;

	diff.x = heroPos.x - pos.x;
	diff.y = heroPos.y - pos.y;
	diff.z = heroPos.z - pos.z;

	//set direction hero, ignore y-axis
	Vector2D objLook(vob->trafoObjToWorld.m[0][2], vob->trafoObjToWorld.m[2][2]);
	Vector2D newLook(heroLook.x, heroLook.z);


	//get angle between old and new look vector and update right vector
	float angle = Vector2D::angleBetween(objLook, newLook);

	//set new look-vector
	vob->trafoObjToWorld.m[0][0] = std::cos(angle);
	vob->trafoObjToWorld.m[0][2] = -std::sin(angle);

	vob->trafoObjToWorld.m[2][0] = std::sin(angle);
	vob->trafoObjToWorld.m[2][2] = std::cos(angle);


	zCVobEndMovement(vob, 1);

	zCVobBeginMovement(vob);

	//zCVobRotateWorldY(vob, angle);

	//do local rotation
	zCVobRotateWorldY(vob, total * 90);

	//vob->SetTrafoObjToWorld(matrix);

	//zMAT4* matrix = vob->trafo;
	//zMAT4PostRotateY(matrix, factor *90);
	//vob->SetPositionWorld(zVEC3(0, 100, 0));
	//vob->trafoObjToWorld = matrix;

	vob->SetPositionWorld(zVEC3(pos.x + factor2*diff.x, pos.y + factor2*diff.y, pos.z + factor2*diff.z));

	zCVobEndMovement(vob, 1);
	//vob->SetCollDetStat(1);
	//vob->MoveWorld(0, 100, 0);
	//vob->SetPositionWorld(zVEC3(0, 0, 0));
	//world->AddVob(vob);
	//zCVobEndMovement(vob, 1);
	//zCVobEndMovement(vob, 0);

}

void TestModule::zCVobDoFrameActivityHook(void* pThis)
{
	//Test((zCVob*)pThis);
	zCVobDoFrameActivity((zCVob*)pThis);
}