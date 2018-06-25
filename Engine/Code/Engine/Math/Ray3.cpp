#include "Engine/Math/Ray3.hpp"

Ray3::Ray3()
	: start()
	, end()
{}

Ray3::Ray3(const Ray3 & copy)
	: start(copy.start)
	, end(copy.end)
{}

Ray3::Ray3(float startX, float startY, float startZ, float endX, float endY, float endZ)
	: start(startX, startY, startZ)
	, end(endX, endY, endZ)
{}

Ray3::Ray3(Vector3 a_start, Vector3 a_end)
	: start(a_start)
	, end(a_end)
{}

/*
Ray3::Ray3(Vector3 a_start, float length, float headingDegrees)
	: start(a_start)
	, end(a_start)
{
	end.SetLengthAndHeadingDegrees(length, headingDegrees);
}*/

Ray3::~Ray3()
{
// 	start = Vector3::ZERO;
// 	end = Vector3::ZERO;
	start = Vector3(0.f, 0.f, 0.f);
	end = Vector3(0.f, 0.f, 0.f);
}

Vector3 Ray3::SetToOrigin()
{
	end -= start;
	//start = Vector3::ZERO;
	start = Vector3(0.f, 0.f, 0.f);
	return end;
}

void Ray3::SetStartAndEnd(const Vector3 & a_start, const Vector3 & a_end)
{
	start = a_start;
	end = a_end;
}

float Ray3::SetLength(float length)
{
	float oldLength = CalcLength();
	end -= start;
	end.SetLength(length);
	end += start;
	return oldLength;
}

float Ray3::AddLength(float lengthToAdd)
{
	float oldLength = CalcLength();
	end -= start;
	end.SetLength(end.CalcLength() + lengthToAdd);
	end += start;
	return oldLength;
}

/*
void Ray3::SetHeadingDegrees(float headingDegrees)
{
	end -= start;
	end.SetHeadingDegrees(headingDegrees);
	end += start;
}

void Ray3::SetHeadingRadians(float headingRadians)
{
	end -= start;
	end.SetHeadingRadians(headingRadians);
	end += start;
}

void Ray3::SetLengthAndHeadingDegrees(float length, float headingDegrees)
{
	end -= start;
	end.SetLengthAndHeadingDegrees(length,headingDegrees);
	end += start;
}

void Ray3::SetLengthAndHeadingRadians(float length, float headingRadians)
{
	end -= start;
	end.SetLengthAndHeadingRadians(length,headingRadians);
	end += start;
}

void Ray3::Rotate90Degrees()
{
	end -= start;
	end.Rotate90Degrees();
	end += start;
}

void Ray3::Ratate180Degrees()
{
	end -= start;
	end.Ratate180Degrees();
	end += start;
}

void Ray3::RotateNegative90Degrees()
{
	end -= start;
	end.RotateNegative90Degrees();
	end += start;
}

void Ray3::RotateDegrees(float rotateDegrees)
{
	end -= start;
	end.RotateDegrees(rotateDegrees);
	end += start;
}

void Ray3::RotateRadians(float rotateRadians)
{
	end -= start;
	end.RotateRadians(rotateRadians);
	end += start;
}*/

void Ray3::Translate(const Vector3 & translation)
{
	start += translation;
	end += translation;
}

void Ray3::ScaleUniform(float scale)
{
	end -= start;
	end.ScaleUniform(scale);
	end += start;
}

void Ray3::ScaleNonUniform(const Vector3 & perAxisScaleFactors)
{
	end -= start;
	end.ScaleNonUniform(perAxisScaleFactors);
	end += start;
}

void Ray3::InverseScaleNonUniform(const Vector3 & perAxisDivisors)
{
	end -= start;
	end.InverseScaleNonUniform(perAxisDivisors);
	end += start;
}

const float Ray3::CalcLength() const
{
	return CalcDistance(start, end);
}

const Vector3 Ray3::CalcCenter() const
{
	return (end + start) * 0.5f;//Vector3((start.x + end.x)*0.5f, (start.y + end.y)*0.5f);
}

const Vector3 Ray3::CalcVector() const
{
	return end - start;
}

const Ray3 Ray3::operator=(const Ray3 & copy)
{
	start = copy.start;
	end = copy.end;
	return *this;
}

bool Ray3::operator==(const Ray3 & rayToEqual) const
{
	return (start == rayToEqual.start) && (end == rayToEqual.end);
}

bool Ray3::operator!=(const Ray3 & rayToNotEqual) const
{
	return !(*this == rayToNotEqual);
}

const Ray3 Ray3::operator+(const Vector3 & translation) const
{
	return Ray3(this->start + translation, this->end + translation);
}

const Ray3 Ray3::operator+(const Ray3 & translation) const
{
	return Ray3(start + translation.start, end + translation.end);
}

const Ray3 Ray3::operator-(const Vector3 & antiTranslation) const
{
	return Ray3(this->start - antiTranslation, this->end - antiTranslation);
}

const Ray3 Ray3::operator*(float scale) const
{
	Ray3 result(*this);
	result.ScaleUniform(scale);
	return result;
}

const Ray3 Ray3::operator*(const Vector3 & perAxisScaleFactors) const
{
	Ray3 result(*this);
	result.ScaleNonUniform(perAxisScaleFactors);
	return result;
}

const Ray3 Ray3::operator/(float inverseScale) const
{
	Ray3 result(*this);
	result.InverseScaleNonUniform(Vector3(inverseScale, inverseScale, inverseScale));
	return result;
}

const Ray3 Ray3::operator/(const Vector3 & perAxisInverseScaleFactors) const
{
	Ray3 result(*this);
	result.InverseScaleNonUniform(perAxisInverseScaleFactors);
	return result;
}

void Ray3::operator*=(float scale)
{
	this->ScaleUniform(scale);
}

void Ray3::operator*=(const Vector3 & perAxisScaleFactors)
{
	this->ScaleNonUniform(perAxisScaleFactors);
}

void Ray3::operator+=(const Vector3 & translation)
{
	this->Translate(translation);
}

void Ray3::operator-=(const Vector3 & antiTranslation)
{
	this->Translate(-1 * antiTranslation);
}

const Ray3 operator*(float scale, const Ray3 & rayToScale)
{
	Ray3 result(rayToScale);
	result *= scale;
	return result;
}

bool IsParallel(const Ray3 & a, const Ray3 & b)
{
	Vector3 ka = a.end - a.start;
	ka.Normalize();
	Vector3 kb = b.end - b.start;
	kb.Normalize();

	return ka == kb;
}
