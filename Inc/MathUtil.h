#pragma once
#include <math.h>
#include <GothicFunctions.h>
#include <api/g2/ztypes.h>


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
		float x = pos.x + lambda * dir.x;
		float y = pos.y + lambda * dir.y;
		float z = pos.z + lambda * dir.z;
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
		zVEC3Normalize(&normal);
		plane.a = normal.x;
		plane.b = normal.y;
		plane.c = normal.z;
		plane.d = (pos.x * normal.x + pos.y * normal.y + pos.z * normal.z);
	};

	float getLengthOfNormalVec() const
	{
		float length = sqrtf(a * a + b * b + c * c);
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
	float getDirectedDistanceToPlane(const zVEC3& point) const
	{
		float normalLength = getLengthOfNormalVec();
		float result = (a * point.x + b * point.y + c * point.z - d) / normalLength;
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
		float scalar = a * dir.x + b * dir.y + c * dir.z;
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
		}
		else
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


	inline float getDistanceOfVectors(const zVEC3& vec1, const zVEC3& vec2) {
		float distance = abs(vec2.x - vec1.x + vec2.y - vec1.y + vec2.z - vec1.z);
		if (Float::areEqual(distance, 0)) {
			return 0;
		}
		return distance;
	}

	inline bool zVEC3IsNull(const zVEC3& vec)
	{
		if (Float::areEqual(vec.x, 0)
			&& Float::areEqual(vec.y, 0)
			&& Float::areEqual(vec.z, 0))
		{
			return true;
		}
		return false;
	}


	friend std::ostream& operator<<(std::ostream& os, const Plane& p);
};