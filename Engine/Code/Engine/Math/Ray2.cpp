#include "Engine/Math/Ray2.hpp"

Ray2::Ray2()
	: start()
	, end()
{}

Ray2::Ray2(const Ray2 & copy)
	: start(copy.start)
	, end(copy.end)
{}

Ray2::Ray2(float startX, float startY, float endX, float endY)
	: start(startX, startY)
	, end(endX, endY)
{}

Ray2::Ray2(Vector2 a_start, Vector2 a_end)
	: start(a_start)
	, end(a_end)
{}

Ray2::Ray2(Vector2 a_start, float length, float headingDegrees)
	: start(a_start)
	, end(a_start)
{
	end.SetLengthAndHeadingDegrees(length, headingDegrees);
}

Ray2::~Ray2()
{
// 	start = Vector2::ZERO;
// 	end = Vector2::ZERO;
	start = Vector2(0.f, 0.f);
	end = Vector2(0.f, 0.f);
}

Vector2 Ray2::SetToOrigin()
{
	end -= start;
	//start = Vector2::ZERO;
	start = Vector2(0.f, 0.f);
	return end;
}

void Ray2::SetStartAndEnd(const Vector2 & a_start, const Vector2 & a_end)
{
	start = a_start;
	end = a_end;
}

float Ray2::SetLength(float length)
{
	float oldLength = CalcLength();
	end -= start;
	end.SetLength(length);
	end += start;
	return oldLength;
}

float Ray2::AddLength(float lengthToAdd)
{
	float oldLength = CalcLength();
	end -= start;
	end.SetLength(end.CalcLength() + lengthToAdd);
	end += start;
	return oldLength;
}

void Ray2::SetHeadingDegrees(float headingDegrees)
{
	end -= start;
	end.SetHeadingDegrees(headingDegrees);
	end += start;
}

void Ray2::SetHeadingRadians(float headingRadians)
{
	end -= start;
	end.SetHeadingRadians(headingRadians);
	end += start;
}

void Ray2::SetLengthAndHeadingDegrees(float length, float headingDegrees)
{
	end -= start;
	end.SetLengthAndHeadingDegrees(length,headingDegrees);
	end += start;
}

void Ray2::SetLengthAndHeadingRadians(float length, float headingRadians)
{
	end -= start;
	end.SetLengthAndHeadingRadians(length,headingRadians);
	end += start;
}

void Ray2::Rotate90Degrees()
{
	end -= start;
	end.Rotate90Degrees();
	end += start;
}

void Ray2::Ratate180Degrees()
{
	end -= start;
	end.Ratate180Degrees();
	end += start;
}

void Ray2::RotateNegative90Degrees()
{
	end -= start;
	end.RotateNegative90Degrees();
	end += start;
}

void Ray2::RotateDegrees(float rotateDegrees)
{
	end -= start;
	end.RotateDegrees(rotateDegrees);
	end += start;
}

void Ray2::RotateRadians(float rotateRadians)
{
	end -= start;
	end.RotateRadians(rotateRadians);
	end += start;
}

void Ray2::Translate(const Vector2 & translation)
{
	start += translation;
	end += translation;
}

void Ray2::ScaleUniform(float scale)
{
	end -= start;
	end.ScaleUniform(scale);
	end += start;
}

void Ray2::ScaleNonUniform(const Vector2 & perAxisScaleFactors)
{
	end -= start;
	end.ScaleNonUniform(perAxisScaleFactors);
	end += start;
}

void Ray2::InverseScaleNonUniform(const Vector2 & perAxisDivisors)
{
	end -= start;
	end.InverseScaleNonUniform(perAxisDivisors);
	end += start;
}

const float Ray2::CalcLength() const
{
	return CalcDistance(start, end);
}

const Vector2 Ray2::CalcCenter() const
{
	return (start + end) * 0.5f;
}

const Vector2 Ray2::CalcVector() const
{
	return end - start;
}

const Ray2 Ray2::operator=(const Ray2 & copy)
{
	start = copy.start;
	end = copy.end;
	return *this;
}

bool Ray2::operator==(const Ray2 & rayToEqual) const
{
	return (start == rayToEqual.start) && (end == rayToEqual.end);
}

bool Ray2::operator!=(const Ray2 & rayToNotEqual) const
{
	return !(*this == rayToNotEqual);
}

const Ray2 Ray2::operator+(const Vector2 & translation) const
{
	return Ray2(this->start + translation, this->end + translation);
}

const Ray2 Ray2::operator+(const Ray2 & translation) const
{
	return Ray2(start + translation.start, end + translation.end);
}

const Ray2 Ray2::operator-(const Vector2 & antiTranslation) const
{
	return Ray2(this->start - antiTranslation, this->end - antiTranslation);
}

const Ray2 Ray2::operator*(float scale) const
{
	Ray2 result(*this);
	result.ScaleUniform(scale);
	return result;
}

const Ray2 Ray2::operator*(const Vector2 & perAxisScaleFactors) const
{
	Ray2 result(*this);
	result.ScaleNonUniform(perAxisScaleFactors);
	return result;
}

const Ray2 Ray2::operator/(float inverseScale) const
{
	Ray2 result(*this);
	result.InverseScaleNonUniform(Vector2(inverseScale, inverseScale));
	return result;
}

const Ray2 Ray2::operator/(const Vector2 & perAxisInverseScaleFactors) const
{
	Ray2 result(*this);
	result.InverseScaleNonUniform(perAxisInverseScaleFactors);
	return result;
}

void Ray2::operator*=(float scale)
{
	this->ScaleUniform(scale);
}

void Ray2::operator*=(const Vector2 & perAxisScaleFactors)
{
	this->ScaleNonUniform(perAxisScaleFactors);
}

void Ray2::operator+=(const Vector2 & translation)
{
	this->Translate(translation);
}

void Ray2::operator-=(const Vector2 & antiTranslation)
{
	this->Translate(-1 * antiTranslation);
}

const Ray2 operator*(float scale, const Ray2 & rayToScale)
{
	Ray2 result(rayToScale);
	result *= scale;
	return result;
}

bool IsParallel(const Ray2 & a, const Ray2 & b)
{
	Vector2 ka = a.end - a.start;
	Vector2 kb = b.end - b.start;

	return ka.x * kb.y == ka.y * kb.x;
}
