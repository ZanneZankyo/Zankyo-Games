#pragma once
#include "Engine\Math\Vector2.hpp"
#include "Engine\Math\Vector3.hpp"
#include "Engine\Math\Vector4.hpp"

class Matrix4
{
private:
	float m_values[16];

public:

	enum RotationOrder : char
	{
		ROTATION_ORDER_XYZ,
		ROTATION_ORDER_XZY,
		ROTATION_ORDER_YZX,
		ROTATION_ORDER_YXZ,
		ROTATION_ORDER_ZXY,
		ROTATION_ORDER_ZYX,
		ROTATION_ORDER_SPHERIC_XYZ,
	};

	Matrix4();
	Matrix4(const Matrix4& copy);

	explicit Matrix4(const float* arrayOfFloats);
	explicit Matrix4(const Vector2& iBasis, const Vector2& jBasis, const Vector2& translation = Vector2(0.f, 0.f));
	explicit Matrix4(const Vector3& iBasis, const Vector3& jBasis, const Vector3& kBasis, const Vector3& translation = Vector3(0.f, 0.f, 0.f));
	explicit Matrix4(const Vector4& iBasis, const Vector4& jBasis, const Vector4& kBasis, const Vector4& translation = Vector4(0.f, 0.f, 0.f, 1.f));

	const Matrix4 operator = (const Matrix4& copy);
	const Matrix4 operator *= (const Matrix4& copy);
	const Matrix4 operator * (const Matrix4& mat) const;
	const Matrix4 operator * (float scale) const;

	Vector3 operator * (const Vector3& vector) const;

	// i = col, j = row
	float GetValue(int i, int j) const;
	float& GetReference(int i, int j);

	void InitializeMatrix();
	void ConcatenateTransform(const Matrix4& other);
	const Matrix4 GetTransformed(const Matrix4& other) const;
	void MakeIdentity();

	const float* GetAsFloatArray() const;
	float* 	 GetAsFloatArray();

	Vector2 TransformPosition(const Vector2& position2D) const; // Assumes z=0, w=1
	Vector3 TransformPosition(const Vector3& position3D) const; // Assumes w=1
	Vector2 TransformDirection(const Vector2& direction2D) const; // Assumes z=0, w=0
	Vector3 TransformDirection(const Vector3& direction3D) const; // Assumes w=0
	Vector4 TransformVector(const Vector4& homogeneousVector) const; // w is provided

	void SetTranslate(const Vector3& translate);
	void SetBasis(const Vector3& i, const Vector3& j, const Vector3& k);
	void GetBasis(Vector4& i, Vector4& j, Vector4& k, Vector4& t) const;

	void Translate(const Vector2& translation2D); // z translation assumed to be 0
	void Translate(const Vector3& translation3D);
	void Scale(float uniformScale);
	void Scale(const Vector2& nonUniformScale2D); // z scale assumed to be 1
	void Scale(const Vector3& nonUniformScale3D);
	void RotateDegreesAboutX(float degrees);
	void RotateDegreesAboutY(float degrees);
	void RotateDegreesAboutZ(float degrees); // Possibly also offered as RotateDegrees2D
	void RotateDegreesAboutAxis(float degrees, const Vector3& basis);
	void RotateRadiansAboutX(float radians);
	void RotateRadiansAboutY(float radians);
	void RotateRadiansAboutZ(float radians); // Possibly also offered as RotateRadians2D
	void RotateRadiansAboutAxis(float radians, const Vector3& basis);

	Matrix4 GetInverse() const;
	Vector3 GetPosition() const;
	Matrix4 GetRotation() const;
	void SetRotation(const Matrix4& rotationMatrix);

	void Decompose(Vector3* outTranslate, Matrix4* outRotaion, Vector3* outScale) const;

	static Matrix4 CreateIdentity(); // z translation assumed to be 0
	static Matrix4 CreateTranslation(const Vector2& translation2D); // z translation assumed to be 0
	static Matrix4 CreateTranslation(const Vector3& translation3D);
	static Matrix4 CreateScale(float uniformScale);
	static Matrix4 CreateScale(const Vector2& nonUniformScale2D); // z scale assumed to be 1
	static Matrix4 CreateScale(const Vector3& nonUniformScale3D);
	static Matrix4 CreateRotationDegreesAboutX(float degrees);
	static Matrix4 CreateRotationDegreesAboutY(float degrees);
	static Matrix4 CreateRotationDegreesAboutZ(float degrees); // a.k.a. CreateRotationDegrees2D
	static Matrix4 CreateRotationDegreesAboutAxis(float degrees, const Vector3& basis);
	static Matrix4 CreateRotationRadiansAboutX(float radians);
	static Matrix4 CreateRotationRadiansAboutY(float radians);
	static Matrix4 CreateRotationRadiansAboutZ(float radians); // a.k.a. CreateRotationRadians2D
	static Matrix4 CreateRotationRadiansAboutAxis(float radians, const Vector3& basis);
	static Matrix4 CreateTransform(const Vector3& translation, const Vector3& rotationDegrees, const Vector3& scale);
	static Matrix4 CreateRotation(const Vector3& rotationDegrees, RotationOrder rotationOrder = ROTATION_ORDER_ZXY);
	static Matrix4 CreateLookAt(const Vector3& from, const Vector3& to, const Vector3& up = Vector3::Y_AXIS);
	static Matrix4 CreateLookAtRotation(const Vector3& from, const Vector3& to, const Vector3& up = Vector3::Y_AXIS);
	static Matrix4 CreateFromToRotation(const Vector3& fromDirection, const Vector3& toDirection);

	static Matrix4 MatrixMakePerspectiveProjection(float fieldOfView, float ratio, float nearZ, float farZ);
	static Matrix4 MatrixMakeOrthoProjection(float nearX, float nearY, float farX, float farY);
	static Matrix4 MatrixMakeOrthoProjection(float width, float height);

	friend const Matrix4 Interpolate(const Matrix4& start, const Matrix4& end, float fractionToEnd);

	static Matrix4 IDENTITY;
	static Vector3 YAW_AXIS;
	static Vector3 PITCH_AXIS;
	static Vector3 ROLL_AXIS;
	
};