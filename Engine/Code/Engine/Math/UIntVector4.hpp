#pragma once
#include <iostream>

class UIntVector4
{
public:
	unsigned int x;
	unsigned int y;
	unsigned int z;
	unsigned int w;

	static const UIntVector4 ZERO;
	static const UIntVector4 ONE;
	static const UIntVector4 UNIT;

public:
	//Constructors
	UIntVector4();
	UIntVector4(const UIntVector4& copy);
	explicit UIntVector4(unsigned int initialX, unsigned int initialY, unsigned int initialZ, unsigned int initialW);

	//Accessors
	void GetXYZW(unsigned int& out_x, unsigned int& out_y, unsigned int& out_z, unsigned int& out_w) const;
	const unsigned int* GetAsArray() const;
	unsigned int* GetAsArray();
	unsigned int CalcLengthSquared() const;

	//Mutators
	void SetXYZW(unsigned int newX, unsigned int newY, unsigned int newZ, unsigned int newW);
	void ScaleUniform(unsigned int scale);
	void ScaleNonUniform(const UIntVector4& perAxisScaleFactors);
	void InverseScaleNonUniform(const UIntVector4& perAxisDivisors);
// 	void SetHeadingDegrees(unsigned int headingDegrees);
// 	void SetHeadingRadians(unsigned int headingRadians);
// 	void SetUnitLengthAndHeadingDegrees(unsigned int headingDegrees);
// 	void SetUnitLengthAndHeadingRadians(unsigned int headingRadians);
// 	void SetLengthAndHeadingDegrees(unsigned int newLength, unsigned int headingDegrees);
// 	void SetLengthAndHeadingRadians(unsigned int newLength, unsigned int headingRadians);
	bool IsMostlyEqualTo(const UIntVector4& comparison, unsigned int epsilon = 0.001f) const;

	//Operators
	const UIntVector4 operator = (const UIntVector4& copy);
	bool operator == (const UIntVector4& vectorToEqual) const;
	bool operator != (const UIntVector4& vectorToNotEqual) const;
	const UIntVector4 operator + (const UIntVector4& vectorToAdd) const;
	const UIntVector4 operator - (const UIntVector4& vectorToSubtract) const;
	const UIntVector4 operator * (unsigned int scale) const;
	const UIntVector4 operator * (const UIntVector4& perAxisScaleFactors) const;
	const UIntVector4 operator / (unsigned int inverseScale) const;
	void operator *= (unsigned int scale);
	void operator *= (const UIntVector4& perAxisScaleFactors);
	void operator /= (unsigned int inverseScale);
	void operator /= (const UIntVector4& perAxisDivisors);
	void operator += (const UIntVector4& vectorToAdd);
	void operator -= (const UIntVector4& vectorToSubtract);

	//static
	//static UIntVector4 GetNormalizeFormDegrees(unsigned int degrees);

	//Standalone "friend" functions
	friend unsigned int CalcDistance(const UIntVector4& positionA, const UIntVector4& positionB);
	friend unsigned int CalcDistanceSquared(const UIntVector4& positionA, const UIntVector4& positionB);
	friend const UIntVector4 operator * (unsigned int scale, const UIntVector4& vectorToScale);
	friend unsigned int DotProduct(const UIntVector4& a, const UIntVector4& b);
	friend bool AreMostlyEqual(const UIntVector4& a, const UIntVector4& b, unsigned int epsilon = 0.001f);
	friend std::ostream& operator << (std::ostream& out ,const UIntVector4& pos);

};