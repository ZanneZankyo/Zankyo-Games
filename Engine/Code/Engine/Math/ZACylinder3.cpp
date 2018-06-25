#include "ZACylinder3.hpp"
#include "Disc2.hpp"

bool ZACylinder::IsPointInside(const Vector3 & pos) const
{
	Vector3 top(GetTopPosition());
	Vector3 bottom(GetBottomPosition());
	if (pos.z > top.z || pos.z < bottom.z)
		return false;
	Disc2 projectionInXYPlane(centerPosition.x, centerPosition.y, radius);
	return projectionInXYPlane.IsPointInside(Vector2(pos));
}

bool DoZACylindersOverlap(const ZACylinder & a, const ZACylinder & b)
{
	if (fabs(a.centerPosition.z - b.centerPosition.z) > (a.height + b.height) * 0.5f)
		return false;
	Disc2 aProjectionInXYPlane(Vector2(a.centerPosition), a.radius);
	Disc2 bProjectionInXYPlane(Vector2(b.centerPosition), b.radius);
	return DoDisc2sOverlap(aProjectionInXYPlane,bProjectionInXYPlane);
}
