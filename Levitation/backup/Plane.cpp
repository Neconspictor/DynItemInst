#include "Plane.h"

std::ostream & operator<<(std::ostream & os, const Plane & p)
{
	os << "(" << std::to_string(p.a) << ", " << std::to_string(p.b) << ", " << std::to_string(p.c) << ", " <<
		std::to_string(p.d) << ")";
	return os;
}