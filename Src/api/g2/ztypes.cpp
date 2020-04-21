#include <api/g2/ztypes.h>

std::ostream& operator<<(std::ostream& os, const zTSpatialState& spatialState)
{
	os << "m_fFloorY = " << spatialState.m_fFloorY << std::endl;
	os << "m_fWaterY = " << spatialState.m_fWaterY << std::endl;
	os << "m_fCeilingY = " << spatialState.m_fCeilingY << std::endl;
	os << "m_fLastFloorY = " << spatialState.m_fLastFloorY << std::endl;

	os << "m_bFloorIsStair = " << (bool)spatialState.m_bFloorIsStair << std::endl;
	os << "m_bFloorIsVob = " << (bool)spatialState.m_bFloorIsVob << std::endl;
	os << "m_bIsUninited = " << (bool)spatialState.m_bIsUninited << std::endl;

	return os;
}