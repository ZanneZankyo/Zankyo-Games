#include "Matrix4.hpp"
#include <vcruntime_string.h>
#include "MathUtils.hpp"

//Column Major
//Get(i,j) , i = col, j = row
/*

I_x J_x K_x T_x | 00 01 02 03 | i0j0 i1j0 i2j0 i3j0
I_y J_y K_y T_y | 04 05 06 07 | i0j1 i1j1 i2j1 i3j1
I_z J_z K_z T_z | 08 09 10 11 | i0j2 i1j2 i2j2 i3j2
I_w J_w K_w T_w | 12 13 14 15 | i0j3 i1j3 i2j3 i3j3

*/

Matrix4 Matrix4::IDENTITY = Matrix4::CreateIdentity();
Vector3 Matrix4::YAW_AXIS = Vector3(0.f, 1.f, 0.f);
Vector3 Matrix4::PITCH_AXIS = Vector3(1.f, 0.f, 0.f);
Vector3 Matrix4::ROLL_AXIS = Vector3(0.f, 0.f, 1.f);



Matrix4 Matrix4::GetInverse() const
{
	double inv[16], det;
	int i;

	inv[0] = m_values[5] * m_values[10] * m_values[15] -
		m_values[5] * m_values[11] * m_values[14] -
		m_values[9] * m_values[6] * m_values[15] +
		m_values[9] * m_values[7] * m_values[14] +
		m_values[13] * m_values[6] * m_values[11] -
		m_values[13] * m_values[7] * m_values[10];

	inv[4] = -m_values[4] * m_values[10] * m_values[15] +
		m_values[4] * m_values[11] * m_values[14] +
		m_values[8] * m_values[6] * m_values[15] -
		m_values[8] * m_values[7] * m_values[14] -
		m_values[12] * m_values[6] * m_values[11] +
		m_values[12] * m_values[7] * m_values[10];

	inv[8] = m_values[4] * m_values[9] * m_values[15] -
		m_values[4] * m_values[11] * m_values[13] -
		m_values[8] * m_values[5] * m_values[15] +
		m_values[8] * m_values[7] * m_values[13] +
		m_values[12] * m_values[5] * m_values[11] -
		m_values[12] * m_values[7] * m_values[9];

	inv[12] = -m_values[4] * m_values[9] * m_values[14] +
		m_values[4] * m_values[10] * m_values[13] +
		m_values[8] * m_values[5] * m_values[14] -
		m_values[8] * m_values[6] * m_values[13] -
		m_values[12] * m_values[5] * m_values[10] +
		m_values[12] * m_values[6] * m_values[9];

	inv[1] = -m_values[1] * m_values[10] * m_values[15] +
		m_values[1] * m_values[11] * m_values[14] +
		m_values[9] * m_values[2] * m_values[15] -
		m_values[9] * m_values[3] * m_values[14] -
		m_values[13] * m_values[2] * m_values[11] +
		m_values[13] * m_values[3] * m_values[10];

	inv[5] = m_values[0] * m_values[10] * m_values[15] -
		m_values[0] * m_values[11] * m_values[14] -
		m_values[8] * m_values[2] * m_values[15] +
		m_values[8] * m_values[3] * m_values[14] +
		m_values[12] * m_values[2] * m_values[11] -
		m_values[12] * m_values[3] * m_values[10];

	inv[9] = -m_values[0] * m_values[9] * m_values[15] +
		m_values[0] * m_values[11] * m_values[13] +
		m_values[8] * m_values[1] * m_values[15] -
		m_values[8] * m_values[3] * m_values[13] -
		m_values[12] * m_values[1] * m_values[11] +
		m_values[12] * m_values[3] * m_values[9];

	inv[13] = m_values[0] * m_values[9] * m_values[14] -
		m_values[0] * m_values[10] * m_values[13] -
		m_values[8] * m_values[1] * m_values[14] +
		m_values[8] * m_values[2] * m_values[13] +
		m_values[12] * m_values[1] * m_values[10] -
		m_values[12] * m_values[2] * m_values[9];

	inv[2] = m_values[1] * m_values[6] * m_values[15] -
		m_values[1] * m_values[7] * m_values[14] -
		m_values[5] * m_values[2] * m_values[15] +
		m_values[5] * m_values[3] * m_values[14] +
		m_values[13] * m_values[2] * m_values[7] -
		m_values[13] * m_values[3] * m_values[6];

	inv[6] = -m_values[0] * m_values[6] * m_values[15] +
		m_values[0] * m_values[7] * m_values[14] +
		m_values[4] * m_values[2] * m_values[15] -
		m_values[4] * m_values[3] * m_values[14] -
		m_values[12] * m_values[2] * m_values[7] +
		m_values[12] * m_values[3] * m_values[6];

	inv[10] = m_values[0] * m_values[5] * m_values[15] -
		m_values[0] * m_values[7] * m_values[13] -
		m_values[4] * m_values[1] * m_values[15] +
		m_values[4] * m_values[3] * m_values[13] +
		m_values[12] * m_values[1] * m_values[7] -
		m_values[12] * m_values[3] * m_values[5];

	inv[14] = -m_values[0] * m_values[5] * m_values[14] +
		m_values[0] * m_values[6] * m_values[13] +
		m_values[4] * m_values[1] * m_values[14] -
		m_values[4] * m_values[2] * m_values[13] -
		m_values[12] * m_values[1] * m_values[6] +
		m_values[12] * m_values[2] * m_values[5];

	inv[3] = -m_values[1] * m_values[6] * m_values[11] +
		m_values[1] * m_values[7] * m_values[10] +
		m_values[5] * m_values[2] * m_values[11] -
		m_values[5] * m_values[3] * m_values[10] -
		m_values[9] * m_values[2] * m_values[7] +
		m_values[9] * m_values[3] * m_values[6];

	inv[7] = m_values[0] * m_values[6] * m_values[11] -
		m_values[0] * m_values[7] * m_values[10] -
		m_values[4] * m_values[2] * m_values[11] +
		m_values[4] * m_values[3] * m_values[10] +
		m_values[8] * m_values[2] * m_values[7] -
		m_values[8] * m_values[3] * m_values[6];

	inv[11] = -m_values[0] * m_values[5] * m_values[11] +
		m_values[0] * m_values[7] * m_values[9] +
		m_values[4] * m_values[1] * m_values[11] -
		m_values[4] * m_values[3] * m_values[9] -
		m_values[8] * m_values[1] * m_values[7] +
		m_values[8] * m_values[3] * m_values[5];

	inv[15] = m_values[0] * m_values[5] * m_values[10] -
		m_values[0] * m_values[6] * m_values[9] -
		m_values[4] * m_values[1] * m_values[10] +
		m_values[4] * m_values[2] * m_values[9] +
		m_values[8] * m_values[1] * m_values[6] -
		m_values[8] * m_values[2] * m_values[5];

	det = m_values[0] * inv[0] + m_values[1] * inv[4] + m_values[2] * inv[8] + m_values[3] * inv[12];

	if (det == 0)
		return Matrix4::IDENTITY;

	det = 1.0 / det;

	Matrix4 result;

	for (i = 0; i < 16; i++)
		result.m_values[i] = (float)(inv[i] * det);

	return result;
}

Matrix4 Matrix4::GetRotation() const
{
	Matrix4 rotation;
	Decompose(nullptr, &rotation, nullptr);
	return rotation;
}

void Matrix4::SetRotation(const Matrix4& rotationMatrix)
{
	Matrix4 rotationMat = rotationMatrix.GetRotation();

	Vector3 translate;
	Vector3 scales;
	Decompose(&translate, nullptr, &scales);

	Matrix4 translateMat = CreateTranslation(translate);
	Matrix4 scaleMat = CreateScale(scales);

	*this = scaleMat * rotationMat * translateMat;

	/*for (int row = 0; row < 4; row++)
		for (int col = 0; col < 3; col++)
		{
			switch (col)
			{
			case 0:
				GetReference(col, row) = rotationMatrix.GetValue(col, row) * scales.x;
				break;
			case 1:
				GetReference(col, row) = rotationMatrix.GetValue(col, row) * scales.y;
				break;
			case 2:
				GetReference(col, row) = rotationMatrix.GetValue(col, row) * scales.z;
				break;
			}
		}*/
}


void Matrix4::Decompose(Vector3* outTranslate, Matrix4* outRotaion, Vector3* outScale) const
{
	if (outTranslate)
	{
		outTranslate->x = GetValue(3, 0);
		outTranslate->y = GetValue(3, 1);
		outTranslate->z = GetValue(3, 2);
	}
	if (!outRotaion && !outScale) // if no rotation and scale query
		return;
	float sX = Vector3(GetValue(0, 0), GetValue(0, 1), GetValue(0, 2)).CalcLength();
	float sY = Vector3(GetValue(1, 0), GetValue(1, 1), GetValue(1, 2)).CalcLength();
	float sZ = Vector3(GetValue(2, 0), GetValue(2, 1), GetValue(2, 2)).CalcLength();
	if (outScale)
	{
		outScale->x = sX;
		outScale->y = sY;
		outScale->z = sZ;
	}
	if (outRotaion)
	{
		for(int row = 0; row < 4; row++)
			for (int col = 0; col < 4; col++)
			{
				switch (col)
				{
				case 0:
					outRotaion->GetReference(col, row) = sX ? GetValue(col, row) / sX : 0.f;
					break;
				case 1:
					outRotaion->GetReference(col, row) = sY ? GetValue(col, row) / sY : 0.f;
					break;
				case 2:
					outRotaion->GetReference(col, row) = sZ ? GetValue(col, row) / sZ : 0.f;
					break;
				case 3:
					outRotaion->GetReference(col, row) = row == 3 ? 1.f : 0.f;
					break;
				default:
					break;
				}
			}
	}
}

/*
Matrix4 Matrix4::GetInverse() const
{
	float det =
		  GetValue(0, 0) * GetValue(1, 1) * GetValue(2, 2) * GetValue(3, 3)
		+ GetValue(0, 0) * GetValue(1, 2) * GetValue(2, 3) * GetValue(3, 1)
		+ GetValue(0, 0) * GetValue(1, 3) * GetValue(2, 1) * GetValue(3, 2)

		+ GetValue(0, 1) * GetValue(1, 0) * GetValue(2, 3) * GetValue(3, 2)
		+ GetValue(0, 1) * GetValue(1, 2) * GetValue(2, 0) * GetValue(3, 3)
		+ GetValue(0, 1) * GetValue(1, 3) * GetValue(2, 2) * GetValue(3, 0)

		+ GetValue(0, 2) * GetValue(1, 0) * GetValue(2, 1) * GetValue(3, 3)
		+ GetValue(0, 2) * GetValue(1, 1) * GetValue(2, 3) * GetValue(3, 0)
		+ GetValue(0, 2) * GetValue(1, 3) * GetValue(2, 0) * GetValue(3, 1)

		+ GetValue(0, 3) * GetValue(1, 0) * GetValue(2, 2) * GetValue(3, 1)
		+ GetValue(0, 3) * GetValue(1, 1) * GetValue(2, 0) * GetValue(3, 2)
		+ GetValue(0, 3) * GetValue(1, 2) * GetValue(2, 1) * GetValue(3, 0)

		- GetValue(0, 0) * GetValue(1, 1) * GetValue(2, 3) * GetValue(3, 2)
		- GetValue(0, 0) * GetValue(1, 2) * GetValue(2, 1) * GetValue(3, 3)
		- GetValue(0, 0) * GetValue(1, 3) * GetValue(2, 2) * GetValue(3, 1)

		- GetValue(0, 1) * GetValue(1, 0) * GetValue(2, 2) * GetValue(3, 3)
		- GetValue(0, 1) * GetValue(1, 2) * GetValue(2, 3) * GetValue(3, 0)
		- GetValue(0, 1) * GetValue(1, 3) * GetValue(2, 0) * GetValue(3, 2)

		- GetValue(0, 2) * GetValue(1, 0) * GetValue(2, 3) * GetValue(3, 1)
		- GetValue(0, 2) * GetValue(1, 1) * GetValue(2, 0) * GetValue(3, 3)
		- GetValue(0, 2) * GetValue(1, 3) * GetValue(2, 1) * GetValue(3, 0)

		- GetValue(0, 3) * GetValue(1, 0) * GetValue(2, 1) * GetValue(3, 2)
		- GetValue(0, 3) * GetValue(1, 1) * GetValue(2, 2) * GetValue(3, 0)
		- GetValue(0, 3) * GetValue(1, 2) * GetValue(2, 0) * GetValue(3, 1);

	if (det)
	{
		Matrix4 result;
		result.GetReference(0, 0) =
			+ GetValue(1, 1) * GetValue(2, 2) * GetValue(3, 3)
			+ GetValue(1, 2) * GetValue(2, 3) * GetValue(3, 1)
			+ GetValue(1, 3) * GetValue(2, 1) * GetValue(3, 2)
			- GetValue(1, 1) * GetValue(2, 3) * GetValue(3, 2)
			- GetValue(1, 2) * GetValue(2, 1) * GetValue(3, 3)
			- GetValue(1, 3) * GetValue(2, 2) * GetValue(3, 1);
		result.GetReference(0, 1) =
			  GetValue(0, 1) * GetValue(2, 3) * GetValue(3, 2)
			+ GetValue(0, 2) * GetValue(2, 1) * GetValue(3, 3)
			+ GetValue(0, 3) * GetValue(2, 2) * GetValue(3, 1)
			- GetValue(0, 1) * GetValue(2, 2) * GetValue(3, 3)
			- GetValue(0, 2) * GetValue(2, 3) * GetValue(3, 1)
			- GetValue(0, 3) * GetValue(2, 1) * GetValue(3, 2);
		result.GetReference(0, 2) =
			+ GetValue(0, 1) * GetValue(1, 2) * GetValue(3, 3)
			+ GetValue(0, 2) * GetValue(1, 3) * GetValue(3, 1)
			+ GetValue(0, 3) * GetValue(1, 1) * GetValue(3, 2)
			- GetValue(0, 1) * GetValue(1, 3) * GetValue(3, 2)
			- GetValue(0, 2) * GetValue(1, 1) * GetValue(3, 3)
			- GetValue(0, 3) * GetValue(1, 2) * GetValue(3, 1);
		result.GetReference(0, 3) =
			+ GetValue(0, 1) * GetValue(1, 3) * GetValue(2, 2)
			+ GetValue(0, 2) * GetValue(1, 1) * GetValue(2, 3)
			+ GetValue(0, 3) * GetValue(1, 2) * GetValue(2, 1)
			- GetValue(0, 1) * GetValue(1, 2) * GetValue(2, 3)
			- GetValue(0, 2) * GetValue(1, 3) * GetValue(2, 1)
			- GetValue(0, 3) * GetValue(1, 1) * GetValue(2, 2);
		result.GetReference(1, 0) =
			+ GetValue(1, 0) * GetValue(2, 3) * GetValue(3, 2)
			+ GetValue(1, 2) * GetValue(2, 0) * GetValue(3, 3)
			+ GetValue(1, 3) * GetValue(2, 2) * GetValue(3, 0)
			- GetValue(1, 0) * GetValue(2, 2) * GetValue(3, 3)
			- GetValue(1, 2) * GetValue(2, 3) * GetValue(3, 1)
			- GetValue(1, 3) * GetValue(2, 0) * GetValue(3, 2);
		result.GetReference(1, 1) =
			+ GetValue(0, 0) * GetValue(2, 2) * GetValue(3, 3)
			+ GetValue(0, 2) * GetValue(2, 3) * GetValue(3, 0)
			+ GetValue(0, 3) * GetValue(2, 0) * GetValue(3, 2)
			- GetValue(0, 0) * GetValue(2, 3) * GetValue(3, 2)
			- GetValue(0, 2) * GetValue(2, 0) * GetValue(3, 3)
			- GetValue(0, 3) * GetValue(2, 2) * GetValue(3, 0);
		result.GetReference(1, 2) =
			+ GetValue(0, 0) * GetValue(1, 3) * GetValue(3, 2)
			+ GetValue(0, 2) * GetValue(1, 0) * GetValue(3, 3)
			+ GetValue(0, 3) * GetValue(1, 2) * GetValue(3, 0)
			- GetValue(0, 0) * GetValue(1, 2) * GetValue(3, 3)
			- GetValue(0, 2) * GetValue(1, 3) * GetValue(3, 0)
			- GetValue(0, 3) * GetValue(1, 0) * GetValue(3, 2);
		result.GetReference(1, 3) =
			+ GetValue(0, 0) * GetValue(1, 2) * GetValue(2, 3)
			+ GetValue(0, 2) * GetValue(1, 3) * GetValue(2, 0)
			+ GetValue(0, 3) * GetValue(1, 0) * GetValue(2, 2)
			- GetValue(0, 0) * GetValue(1, 3) * GetValue(2, 2)
			- GetValue(0, 2) * GetValue(1, 0) * GetValue(2, 3)
			- GetValue(0, 3) * GetValue(1, 2) * GetValue(2, 0);
		result.GetReference(2, 0) =
			+ GetValue(2, 0) * GetValue(2, 1) * GetValue(3, 3)
			+ GetValue(1, 1) * GetValue(2, 3) * GetValue(3, 0)
			+ GetValue(1, 3) * GetValue(2, 0) * GetValue(3, 1)
			- GetValue(1, 0) * GetValue(2, 3) * GetValue(3, 1)
			- GetValue(1, 1) * GetValue(2, 0) * GetValue(3, 3)
			- GetValue(1, 3) * GetValue(2, 1) * GetValue(3, 0);
		result.GetReference(2, 1) =
			+ GetValue(0, 0) * GetValue(2, 3) * GetValue(3, 1)
			+ GetValue(0, 1) * GetValue(2, 0) * GetValue(3, 3)
			+ GetValue(0, 3) * GetValue(2, 1) * GetValue(3, 0)
			- GetValue(0, 0) * GetValue(2, 1) * GetValue(3, 3)
			- GetValue(0, 1) * GetValue(2, 3) * GetValue(3, 0)
			- GetValue(0, 3) * GetValue(2, 0) * GetValue(3, 1);
		result.GetReference(2, 2) =
			+ GetValue(0, 0) * GetValue(1, 1) * GetValue(3, 3)
			+ GetValue(0, 1) * GetValue(1, 3) * GetValue(3, 0)
			+ GetValue(0, 3) * GetValue(1, 0) * GetValue(3, 1)
			- GetValue(0, 0) * GetValue(1, 3) * GetValue(3, 1)
			- GetValue(0, 1) * GetValue(1, 0) * GetValue(3, 3)
			- GetValue(0, 3) * GetValue(1, 1) * GetValue(3, 0);
		result.GetReference(2, 3) =
			+ GetValue(0, 0) * GetValue(1, 3) * GetValue(2, 1)
			+ GetValue(0, 1) * GetValue(1, 0) * GetValue(2, 3)
			+ GetValue(0, 3) * GetValue(1, 1) * GetValue(2, 0)
			- GetValue(0, 0) * GetValue(1, 1) * GetValue(2, 3)
			- GetValue(0, 1) * GetValue(1, 3) * GetValue(2, 0)
			- GetValue(0, 3) * GetValue(1, 0) * GetValue(2, 1);
		result.GetReference(3, 0) =
			+ GetValue(1, 0) * GetValue(2, 2) * GetValue(3, 1)
			+ GetValue(1, 1) * GetValue(2, 0) * GetValue(3, 2)
			+ GetValue(1, 2) * GetValue(2, 1) * GetValue(3, 0)
			- GetValue(1, 0) * GetValue(3, 1) * GetValue(3, 2)
			- GetValue(1, 1) * GetValue(2, 2) * GetValue(3, 0)
			- GetValue(1, 2) * GetValue(2, 0) * GetValue(3, 1);
		result.GetReference(3, 1) =
			+ GetValue(0, 0) * GetValue(2, 1) * GetValue(3, 2)
			+ GetValue(0, 1) * GetValue(2, 2) * GetValue(3, 0)
			+ GetValue(0, 2) * GetValue(2, 0) * GetValue(3, 1)
			- GetValue(0, 0) * GetValue(2, 2) * GetValue(3, 1)
			- GetValue(0, 1) * GetValue(2, 0) * GetValue(3, 2)
			- GetValue(0, 2) * GetValue(2, 1) * GetValue(3, 0);
		result.GetReference(3, 2) =
			+ GetValue(0, 0) * GetValue(1, 2) * GetValue(3, 1)
			+ GetValue(0, 1) * GetValue(1, 0) * GetValue(3, 2)
			+ GetValue(0, 2) * GetValue(1, 1) * GetValue(3, 0)
			- GetValue(0, 0) * GetValue(1, 1) * GetValue(3, 2)
			- GetValue(0, 1) * GetValue(1, 2) * GetValue(3, 0)
			- GetValue(0, 2) * GetValue(1, 0) * GetValue(3, 1);
		result.GetReference(3, 3) =
			+ GetValue(0, 0) * GetValue(1, 1) * GetValue(2, 2)
			+ GetValue(0, 1) * GetValue(1, 2) * GetValue(2, 0)
			+ GetValue(0, 2) * GetValue(1, 0) * GetValue(2, 1)
			- GetValue(0, 0) * GetValue(1, 2) * GetValue(2, 1)
			- GetValue(0, 1) * GetValue(1, 0) * GetValue(2, 2)
			- GetValue(0, 2) * GetValue(1, 1) * GetValue(2, 0);
		return result * det;
	}
	return Matrix4::IDENTITY;
}*/

Matrix4::Matrix4()
{
	InitializeMatrix();
}

Matrix4::Matrix4(const Matrix4 & copy)
{
	for (int index = 0; index < 16; index++)
		m_values[index] = copy.m_values[index];
}

Matrix4::Matrix4(const float * arrayOfFloats)
{
	for (int index = 0; index < 16; index++)
		m_values[index] = arrayOfFloats[index];
}

Matrix4::Matrix4(const Vector2 & iBasis, const Vector2 & jBasis, const Vector2 & translation)
{
	MakeIdentity();
	GetReference(0, 0) = iBasis.x;
	GetReference(0, 1) = iBasis.y;

	GetReference(1, 0) = jBasis.x;
	GetReference(1, 1) = jBasis.y;

	GetReference(3, 0) = translation.x;
	GetReference(3, 1) = translation.y;
}

Matrix4::Matrix4(const Vector3 & iBasis, const Vector3 & jBasis, const Vector3 & kBasis, const Vector3 & translation)
{
	MakeIdentity();

	GetReference(0, 0) = iBasis.x;
	GetReference(0, 1) = iBasis.y;
	GetReference(0, 2) = iBasis.z;

	GetReference(1, 0) = jBasis.x;
	GetReference(1, 1) = jBasis.y;
	GetReference(1, 2) = jBasis.z;

	GetReference(2, 0) = kBasis.x;
	GetReference(2, 1) = kBasis.y;
	GetReference(2, 2) = kBasis.z;

	GetReference(3, 0) = translation.x;
	GetReference(3, 1) = translation.y;
	GetReference(3, 2) = translation.z;

}

Matrix4::Matrix4(const Vector4 & iBasis, const Vector4 & jBasis, const Vector4 & kBasis, const Vector4 & translation)
{
	MakeIdentity();

	GetReference(0, 0) = iBasis.x;
	GetReference(0, 1) = iBasis.y;
	GetReference(0, 2) = iBasis.z;
	GetReference(0, 3) = iBasis.w;

	GetReference(1, 0) = jBasis.x;
	GetReference(1, 1) = jBasis.y;
	GetReference(1, 2) = jBasis.z;
	GetReference(1, 3) = jBasis.w;

	GetReference(2, 0) = kBasis.x;
	GetReference(2, 1) = kBasis.y;
	GetReference(2, 2) = kBasis.z;
	GetReference(2, 3) = kBasis.w;

	GetReference(3, 0) = translation.x;
	GetReference(3, 1) = translation.y;
	GetReference(3, 2) = translation.z;
	GetReference(3, 3) = translation.w;
}

const Matrix4 Matrix4::operator*=(const Matrix4& copy)
{
	this->ConcatenateTransform(copy);
	return *this;
}

const Matrix4 Matrix4::operator*(float scale) const
{
	Matrix4 newMat(*this);
	for (float& value : newMat.m_values)
		value *= scale;
	return newMat;
}

Vector3 Matrix4::operator*(const Vector3& vector) const
{
	return TransformPosition(vector);
}

float Matrix4::GetValue(int i, int j) const
{
	int index = i + j * 4;
	return m_values[index];
}

float & Matrix4::GetReference(int i, int j)
{
	int index = i + j * 4;
	return m_values[index];
}

void Matrix4::InitializeMatrix()
{
	//memset(m_values, 0, static_cast<size_t>(16));
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{
			if (i == j)
				m_values[i * 4 + j] = 1;
			else
				m_values[i * 4 + j] = 0;
		}
}

void Matrix4::ConcatenateTransform(const Matrix4 & other)
{
	*this = GetTransformed(other);
}

const Matrix4 Matrix4::GetTransformed(const Matrix4 & other) const
{
	Matrix4 temp;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			temp.GetReference(i, j) = GetValue(0, j) * other.GetValue(i, 0)
									+ GetValue(1, j) * other.GetValue(i, 1)
									+ GetValue(2, j) * other.GetValue(i, 2)
									+ GetValue(3, j) * other.GetValue(i, 3);
// 			temp.GetReference(i, j) = 
// 				  GetValue(i, 0) * other.GetValue(0, j)
// 				+ GetValue(i, 1) * other.GetValue(1, j)
// 				+ GetValue(i, 2) * other.GetValue(2, j)
// 				+ GetValue(i, 3) * other.GetValue(3, j);
		}
	}
	return temp;
}

void Matrix4::MakeIdentity()
{
	memset(m_values, 0, sizeof(float) * 16);
	GetReference(0, 0) = 1;
	GetReference(1, 1) = 1;
	GetReference(2, 2) = 1;
	GetReference(3, 3) = 1;
}

const float * Matrix4::GetAsFloatArray() const
{
	return m_values;
}

float * Matrix4::GetAsFloatArray()
{
	return m_values;
}

Vector2 Matrix4::TransformPosition(const Vector2 & position2D) const
{
	Vector4 homogeneousVector = Vector4(position2D.x, position2D.y, 0.f, 1.f);
	Vector4 result = TransformVector(homogeneousVector);
	return Vector2(result.x, result.y);
}

Vector3 Matrix4::TransformPosition(const Vector3 & position3D) const
{
	Vector4 homogeneousVector = Vector4(position3D.x, position3D.y, position3D.z, 1.f);
	Vector4 result = TransformVector(homogeneousVector);
	return Vector3(result.x, result.y, result.z);
}

Vector2 Matrix4::TransformDirection(const Vector2 & direction2D) const
{
	Vector4 homogeneousVector = Vector4(direction2D.x, direction2D.y, 0.f, 0.f);
	Vector4 result = TransformVector(homogeneousVector);
	return Vector2(result.x, result.y);
}

Vector3 Matrix4::TransformDirection(const Vector3 & direction3D) const
{
	Vector4 homogeneousVector = Vector4(direction3D.x, direction3D.y, direction3D.z, 0.f);
	Vector4 result = TransformVector(homogeneousVector);
	return Vector3(result.x, result.y, result.z);
}

Vector4 Matrix4::TransformVector(const Vector4 & homogeneousVector) const
{
	/*float x = homogeneousVector.x * GetValue(0, 0) + homogeneousVector.y * GetValue(0, 1) + homogeneousVector.z * GetValue(0, 2) + homogeneousVector.w * GetValue(0, 3);
	float y = homogeneousVector.x * GetValue(1, 0) + homogeneousVector.y * GetValue(1, 1) + homogeneousVector.z * GetValue(1, 2) + homogeneousVector.w * GetValue(1, 3);
	float z = homogeneousVector.x * GetValue(2, 0) + homogeneousVector.y * GetValue(2, 1) + homogeneousVector.z * GetValue(2, 2) + homogeneousVector.w * GetValue(2, 3);
	float w = homogeneousVector.x * GetValue(3, 0) + homogeneousVector.y * GetValue(3, 1) + homogeneousVector.z * GetValue(3, 2) + homogeneousVector.w * GetValue(3, 3);
	*/
	float x = homogeneousVector.x * GetValue(0, 0) + homogeneousVector.y * GetValue(1, 0) + homogeneousVector.z * GetValue(2, 0) + homogeneousVector.w * GetValue(3, 0);
	float y = homogeneousVector.x * GetValue(0, 1) + homogeneousVector.y * GetValue(1, 1) + homogeneousVector.z * GetValue(2, 1) + homogeneousVector.w * GetValue(3, 1);
	float z = homogeneousVector.x * GetValue(0, 2) + homogeneousVector.y * GetValue(1, 2) + homogeneousVector.z * GetValue(2, 2) + homogeneousVector.w * GetValue(3, 2);
	float w = homogeneousVector.x * GetValue(0, 3) + homogeneousVector.y * GetValue(1, 3) + homogeneousVector.z * GetValue(2, 3) + homogeneousVector.w * GetValue(3, 3);

	return Vector4(x,y,z,w);
}

Vector3 Matrix4::GetPosition() const
{
	return Vector3(GetValue(3, 0), GetValue(3, 1), GetValue(3, 2));
}

void Matrix4::SetTranslate(const Vector3 & translate)
{
	GetReference(3, 0) = translate.x;
	GetReference(3, 1) = translate.y;
	GetReference(3, 2) = translate.z;
}

void Matrix4::SetBasis(const Vector3 & i, const Vector3 & j, const Vector3 & k)
{
	GetReference(0, 0) = i.x;
	GetReference(0, 1) = i.y;
	GetReference(0, 2) = i.z;
	GetReference(1, 0) = j.x;
	GetReference(1, 1) = j.y;
	GetReference(1, 2) = j.z;
	GetReference(2, 0) = k.x;
	GetReference(2, 1) = k.y;
	GetReference(2, 2) = k.z;
}

void Matrix4::GetBasis(Vector4& i, Vector4& j, Vector4& k, Vector4& t) const
{
	i.x = GetValue(0, 0);
	i.y = GetValue(0, 1);
	i.z = GetValue(0, 2);
	i.w = GetValue(0, 3);
	j.x = GetValue(1, 0);
	j.y = GetValue(1, 1);
	j.z = GetValue(1, 2);
	j.w = GetValue(1, 3);
	k.x = GetValue(2, 0);
	k.y = GetValue(2, 1);
	k.z = GetValue(2, 2);
	k.w = GetValue(2, 3);
	t.x = GetValue(3, 0);
	t.y = GetValue(3, 1);
	t.z = GetValue(3, 2);
	t.w = GetValue(3, 3);
}

void Matrix4::Translate(const Vector2 & translation2D)
{
	Matrix4 mat(CreateTranslation(translation2D));
	ConcatenateTransform(mat);
}

void Matrix4::Translate(const Vector3 & translation3D)
{
	Matrix4 mat(CreateTranslation(translation3D));
	ConcatenateTransform(mat);
}

void Matrix4::Scale(float uniformScale)
{
	Matrix4 mat(CreateScale(uniformScale));
	ConcatenateTransform(mat);
}

void Matrix4::Scale(const Vector2 & nonUniformScale2D)
{
	Matrix4 mat(CreateScale(nonUniformScale2D));
	ConcatenateTransform(mat);
}

void Matrix4::Scale(const Vector3 & nonUniformScale3D)
{
	Matrix4 mat(CreateScale(nonUniformScale3D));
	ConcatenateTransform(mat);
}

void Matrix4::RotateDegreesAboutX(float degrees)
{
	RotateRadiansAboutX(ConvertDegreesToRadians(degrees));
}

void Matrix4::RotateDegreesAboutY(float degrees)
{
	RotateRadiansAboutY(ConvertDegreesToRadians(degrees));
}

void Matrix4::RotateDegreesAboutZ(float degrees)
{
	RotateRadiansAboutZ(ConvertDegreesToRadians(degrees));
}

void Matrix4::RotateDegreesAboutAxis(float degrees, const Vector3 & basis)
{
	RotateRadiansAboutAxis(ConvertDegreesToRadians(degrees), basis);
}

void Matrix4::RotateRadiansAboutX(float radians)
{
	Matrix4 mat(CreateRotationRadiansAboutX(radians));
	ConcatenateTransform(mat);
}

void Matrix4::RotateRadiansAboutY(float radians)
{
	Matrix4 mat(CreateRotationRadiansAboutY(radians));
	ConcatenateTransform(mat);
}

void Matrix4::RotateRadiansAboutZ(float radians)
{
	Matrix4 mat(CreateRotationRadiansAboutZ(radians));
	ConcatenateTransform(mat);
}

void Matrix4::RotateRadiansAboutAxis(float radians, const Vector3 & basis)
{
	Matrix4 mat(CreateRotationRadiansAboutAxis(radians, basis));
	ConcatenateTransform(mat);
}

Matrix4 Matrix4::CreateIdentity()
{
	Matrix4 mat;
	mat.MakeIdentity();
	return mat;
}

Matrix4 Matrix4::CreateTranslation(const Vector2 & translation2D)
{
	Matrix4 mat(CreateIdentity());
	mat.GetReference(3, 0) = translation2D.x;
	mat.GetReference(3, 1) = translation2D.y;
	return mat;
}

Matrix4 Matrix4::CreateTranslation(const Vector3 & translation3D)
{
	Matrix4 mat(CreateIdentity());
	mat.GetReference(3, 0) = translation3D.x;
	mat.GetReference(3, 1) = translation3D.y;
	mat.GetReference(3, 2) = translation3D.z;
	return mat;
}

Matrix4 Matrix4::CreateScale(float uniformScale)
{
	Matrix4 mat(CreateIdentity());
	mat.GetReference(0, 0) = uniformScale;
	mat.GetReference(1, 1) = uniformScale;
	mat.GetReference(2, 2) = uniformScale;
	return mat;
}

Matrix4 Matrix4::CreateScale(const Vector2 & nonUniformScale2D)
{
	Matrix4 mat(CreateIdentity());
	mat.GetReference(0, 0) = nonUniformScale2D.x;
	mat.GetReference(1, 1) = nonUniformScale2D.y;
	return mat;
}

Matrix4 Matrix4::CreateScale(const Vector3 & nonUniformScale3D)
{
	Matrix4 mat(CreateIdentity());
	mat.GetReference(0, 0) = nonUniformScale3D.x;
	mat.GetReference(1, 1) = nonUniformScale3D.y;
	mat.GetReference(2, 2) = nonUniformScale3D.z;
	return mat;
}

Matrix4 Matrix4::CreateRotationDegreesAboutX(float degrees)
{
	return CreateRotationRadiansAboutX(ConvertDegreesToRadians(degrees));
}

Matrix4 Matrix4::CreateRotationDegreesAboutY(float degrees)
{
	return CreateRotationRadiansAboutY(ConvertDegreesToRadians(degrees));
}

Matrix4 Matrix4::CreateRotationDegreesAboutZ(float degrees)
{
	return CreateRotationRadiansAboutZ(ConvertDegreesToRadians(degrees));
}

Matrix4 Matrix4::CreateRotationDegreesAboutAxis(float degrees, const Vector3 & basis)
{
	return Matrix4::CreateRotationRadiansAboutAxis(ConvertDegreesToRadians(degrees), basis);
}

Matrix4 Matrix4::CreateRotationRadiansAboutX(float radians)
{
	Matrix4 mat(CreateIdentity());
	float cosTheta = cos(radians);
	float sinTheta = sin(radians);
	mat.GetReference(1, 1) = cosTheta;
	mat.GetReference(2, 2) = cosTheta;
	mat.GetReference(1, 2) = sinTheta;
	mat.GetReference(2, 1) = -sinTheta;
	//mat.GetReference(1, 2) = -sinTheta;
	//mat.GetReference(2, 1) = sinTheta;
	return mat;
}

Matrix4 Matrix4::CreateRotationRadiansAboutY(float radians)
{
	Matrix4 mat(CreateIdentity());
	float cosTheta = cos(radians);
	float sinTheta = sin(radians);
	mat.GetReference(0, 0) = cosTheta;
	mat.GetReference(2, 2) = cosTheta;
	mat.GetReference(0, 2) = -sinTheta;
	mat.GetReference(2, 0) = sinTheta;
	//mat.GetReference(0, 2) = sinTheta;
	//mat.GetReference(2, 0) = -sinTheta;
	return mat;
}

Matrix4 Matrix4::CreateRotationRadiansAboutZ(float radians)
{
	Matrix4 mat(CreateIdentity());
	float cosTheta = cos(radians);
	float sinTheta = sin(radians);
	mat.GetReference(0, 0) = cosTheta;
	mat.GetReference(1, 1) = cosTheta;
	mat.GetReference(1, 0) = -sinTheta;
	mat.GetReference(0, 1) = sinTheta;
	//mat.GetReference(1, 0) = sinTheta;
	//mat.GetReference(0, 1) = -sinTheta;
	return mat;
}

Matrix4 Matrix4::CreateRotationRadiansAboutAxis(float radians, const Vector3 & basis)
{
	Matrix4 mat(CreateIdentity());
	float cosR = cos(radians);
	float sinR = sin(radians);
	float x = basis.x;
	float y = basis.y;
	float z = basis.z;

	mat.GetReference(0, 0) = cosR + x*x*(1 - cosR);
	mat.GetReference(1, 0) = x*y*(1 - cosR) - z*sinR;
	mat.GetReference(2, 0) = x*z*(1 - cosR) + y*sinR;

	mat.GetReference(0, 1) = y*x*(1 - cosR) + z*sinR;
	mat.GetReference(1, 1) = cosR + y*y*(1 - cosR);
	mat.GetReference(2, 1) = y*z*(1 - cosR) - x*sinR;

	mat.GetReference(0, 2) = z*x*(1 - cosR) - y*sinR;
	mat.GetReference(1, 2) = z*y*(1 - cosR) + x*sinR;
	mat.GetReference(2, 2) = cosR + z*z*(1 - cosR);

	return mat;
}

Matrix4 Matrix4::CreateTransform(const Vector3& translation, const Vector3& degreesRollPitchYaw, const Vector3& scale)
{
	Matrix4 mat = Matrix4::IDENTITY;
	mat.Scale(scale);
	mat = mat * CreateRotation(degreesRollPitchYaw);
	mat = mat * CreateTranslation(translation);
	//mat.Translate(translation);

	return mat;
}

Matrix4 Matrix4::CreateRotation(const Vector3& rotationDegrees, RotationOrder rotationOrder)
{

	Matrix4 mat(Matrix4::IDENTITY);

	Matrix4 rX = Matrix4::CreateRotationDegreesAboutX(rotationDegrees.x);
	Matrix4 rY = Matrix4::CreateRotationDegreesAboutY(rotationDegrees.y);
	Matrix4 rZ = Matrix4::CreateRotationDegreesAboutZ(rotationDegrees.z);

	switch (rotationOrder)
	{
	case ROTATION_ORDER_XYZ:
		mat = mat * rZ * rY * rX;
		break;
	case ROTATION_ORDER_XZY:
		mat = mat * rY * rZ * rX;
		break;
	case ROTATION_ORDER_YZX:
		mat = mat * rX * rZ * rY;
		break;
	case ROTATION_ORDER_YXZ:
		mat = mat * rZ * rX * rY;
		break;
	case ROTATION_ORDER_ZXY:
		mat = mat * rY * rX * rZ;
		break;
	case ROTATION_ORDER_ZYX:
		mat = mat * rX * rY * rZ;
		break;
	case ROTATION_ORDER_SPHERIC_XYZ:
		break;
	default:
		break;
	}
	return mat;
}

Matrix4 Matrix4::CreateLookAt(const Vector3& from, const Vector3& to, const Vector3& up)
{
	/*Vector3 k = from - to;
	k.Normalize();
	Vector3 i = CrossProduct(k, Vector3::Y_AXIS);
	i.Normalize();
	Vector3 j = CrossProduct(k, i);
	j.Normalize();
	Matrix4 matT;//(Matrix4::CreateIdentity());

	matT.Translate(to);

	Matrix4 matR;

	matR.SetBasis(k, i, j);

	matT.ConcatenateTransform(matR);
	//matT.RotateDegreesAboutX(m_rotateFraction * 360.f);
	return matT;*/
	Vector3 zaxis = (to - from).GetNormalize();
	Vector3 xaxis = CrossProduct(up, zaxis).GetNormalize();
	Vector3 yaxis = CrossProduct(zaxis, xaxis);
	Vector3 translate(-DotProduct(xaxis, from), -DotProduct(yaxis, from), -DotProduct(zaxis, from));

	Vector3 i(xaxis.x, yaxis.x, zaxis.x);
	Vector3 j(xaxis.y, yaxis.y, zaxis.y);
	Vector3 k(xaxis.z, yaxis.z, zaxis.z);

	return Matrix4(i, j, k, translate);

		//row major formular
		/*
		xaxis.x						yaxis.x						zaxis.x						0
		xaxis.y						yaxis.y						zaxis.y						0
		xaxis.z						yaxis.z						zaxis.z						0
		-dot(xaxis,cameraPosition)	-dot(yaxis,cameraPosition)	-dot(zaxis,cameraPosition)  1
		*/
}

Matrix4 Matrix4::CreateLookAtRotation(const Vector3& from, const Vector3& to, const Vector3& up)
{
	Vector3 zaxis = (to - from).GetNormalize();
	Vector3 xaxis = CrossProduct(up, zaxis).GetNormalize();
	Vector3 yaxis = CrossProduct(zaxis, xaxis);
	//Vector3 translate(-DotProduct(xaxis, from), -DotProduct(yaxis, from), -DotProduct(zaxis, from));

	Vector3 i(xaxis.x, yaxis.x, zaxis.x);
	Vector3 j(xaxis.y, yaxis.y, zaxis.y);
	Vector3 k(xaxis.z, yaxis.z, zaxis.z);

	return Matrix4(i, j, k, Vector3::ZERO);
}

Matrix4 Matrix4::CreateFromToRotation(const Vector3& fromDirection, const Vector3& toDirection)
{
	Matrix4 fromRotation = CreateLookAtRotation(Vector3::ZERO, fromDirection);
	Matrix4 toRotation = CreateLookAtRotation(Vector3::ZERO, toDirection);
	return toRotation * fromRotation.GetInverse();
}

Matrix4 Matrix4::MatrixMakePerspectiveProjection(float fieldOfView, float ratio, float nearZ, float farZ)
{
	float fovRad = ConvertDegreesToRadians(fieldOfView);
	float size = 1.0f / tanf(fovRad / 2.0f);

	// scale X or Y depending which dimension is bigger
	float w = size;
	float h = size;
	if (ratio > 1.0f) {
		w /= ratio;
	}
	else {
		h *= ratio;
	}

	float q = 1.0f / (farZ - nearZ);

	float projection[16] = {
		w, 0.0f, 0.0f, 0.0f,
		0.0f, h, 0.0f, 0.0f,
		0.0f, 0.0f, farZ * q, -nearZ * farZ * q,
		0.0f, 0.0f, 1.0f, 0.0f
	};
	return Matrix4(projection);
// 	Vector4 i(w, 0.0f, 0.0f, 0.0f);
// 	Vector4 j(0.0f, h, 0.0f, 0.0f);
// 	Vector4 k(0.0f, 0.0f, farZ * q, 1.0f);
// 	Vector4 t(0.0f, 0.0f, -nearZ * farZ * q, 0.0f);
// 
//	return Matrix4(i,j,k,t);
}

Matrix4 Matrix4::MatrixMakeOrthoProjection(float width, float height)
{
	return MatrixMakeOrthoProjection(0, 0, width, height);
}

Matrix4 Matrix4::MatrixMakeOrthoProjection(float nearX, float nearY, float farX, float farY)
{
	float nx = nearX;
	float fx = farX;
	float ny = nearY;
	float fy = farY;
	float nz = -10.f;
	float fz = 10.f;

	float sx = 1.f / (fx - nx);
	float sy = 1.f / (fy - ny);
	float sz = 1.f / (fz - nz);

	float projection[16] = {

		2.f * sx,			0.f,				0.f,		-(fx + nx) * sx,
		0.f,				2.f * sy,			0.f,		-(fy + ny) * sy,
		0.f,				0.f,				1.f * sz,	-nz * sz,
		0.f,				0.f,				0.f,		1.f

	};

	return Matrix4(projection);
}

const Matrix4 Matrix4::operator=(const Matrix4 & copy)
{
	for (int index = 0; index < 16; index++)
		m_values[index] = copy.m_values[index];
	return *this;
}

const Matrix4 Matrix4::operator*(const Matrix4 & mat) const
{
	return mat.GetTransformed(*this);
}

const Matrix4 Interpolate(const Matrix4 & start, const Matrix4 & end, float fractionToEnd)
{
	float arr[16];
	for (int index = 0; index < 16; index++)
		arr[index] = Interpolate(start.m_values[index], end.m_values[index], fractionToEnd);
	return Matrix4(arr);
}
