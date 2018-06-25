#include "PhysicsTypes.hpp"

#ifdef TOOLS_BUILD

Vector3 ToVector3(const PxVec3& vec)
{
	return Vector3(vec.x, vec.y, vec.z);
}

Vector4 ToVector4(const PxVec4& vec)
{
	return Vector4(vec.x, vec.y, vec.z, vec.w);
}

Matrix4 ToMatrix4(const PxTransform& transform)
{
	PxMat44 pxMat(transform);
	return Matrix4(ToVector4(pxMat.column0), ToVector4(pxMat.column1), ToVector4(pxMat.column2), ToVector4(pxMat.column3));
}

physx::PxVec3 ToPxVec3(const Vector3& vec)
{
	return PxVec3(vec.x, vec.y, vec.z);
}

physx::PxVec4 ToPxVec4(const Vector4& vec)
{
	return PxVec4(vec.x, vec.y, vec.z, vec.w);
}

physx::PxTransform ToPxTransform(const Matrix4& transform)
{
	//Vector3 translate;
	//Matrix4 rotation;
	//transform.Decompose(&translate, &rotation, nullptr);
	//Matrix4 unitTransform = rotation * Matrix4::CreateTranslation(translate);
	Vector4 i, j, k, t;
	transform.GetBasis(i, j, k, t);
	PxMat44 pxMat(ToPxVec4(i), ToPxVec4(j), ToPxVec4(k), ToPxVec4(t));
	PxTransform pxTransform(pxMat);
	return pxTransform.getNormalized();
}

#endif