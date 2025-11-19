#pragma once
#include <cmath>
#include <raylib.h>

namespace cart
{
	Vector2 AngleToVector(float rotation);

	float VectorToRotation(Vector2 vec1, Vector2 vec2);

	float DegreesToRadians(float degrees);

	float RadiansToDegrees(float radians);

    unsigned char LerpChar(unsigned char a, unsigned char  b, unsigned char  alpha);


    Vector2 LerpVector(const Vector2& a, const Vector2& b, float alpha);

	float LerpFloat(float a, float b, float alpha);

	float RandomRange(float min, float max);

	Color RandomColor(Color col1, Color col2);

	Vector2 RandomUnitVector();

    float GetVectorLength(Vector2 vector);

    float GetRawVectorLength(Vector2 vector);

    Vector2 NormailzeVec(Vector2 vector);

	Vector2 Direction(const Vector2& begin, const Vector2& end);

	float Dot(const Vector2& v1, const Vector2& v2);

	Vector2& ScaleVector(Vector2 &vectorToScale, float amt);

	float LERP(float _a, float _b, float _t);

	Vector2 LERP(Vector2 _a, Vector2 _b, float _t);
	
	Vector3 LERP(Vector3 _a, Vector3 _b, float _t);

	Color LERP(Color _a, Color _b, float _t , bool _alpha);

	Vector2 GetPointInCircle();
	Vector2 GetPointOnRectangle();
	Vector2 GetPointOnCircle();
}