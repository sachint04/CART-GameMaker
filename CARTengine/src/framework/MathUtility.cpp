#include <raylib.h>
#include <cstdio>
#include <random>
#include "MathUtility.h"
#include "Core.h"

namespace cart
{

	Vector2 AngleToVector(float rotation) {
		float radians = DegreesToRadians(rotation);
		Vector2 vec {std::cos(radians), std::sin(radians)};
		return vec;
	}

	Vector2 GetPointInCircle() {
		float ran = RandomRange(0.f, 1.f) * 360.f;
		float radian = DegreesToRadians(ran);
		float x = std::cos(radian) ;
		float y = std::sin(radian);
		Vector2 p = { x,y };
		return p;
	}

	Vector2 GetPointOnRectangle()
	{
		int seed = std::round(RandomRange(0.f, 1.f));
		float rany = RandomRange(-1.f, 1.f);
		float ranx = RandomRange(-1.f, 1.f);
		
		if (seed == 0) 
		{
			rany = std::round(rany);
		}
		else {
			ranx = std::round(ranx);
		}
		Vector2 rect = { ranx, rany};
		return rect;

	}

	float VectorToRotation(Vector2 vec1 , Vector2 vec2) {
		float dot = Dot(vec1, vec2);
		float vecLen1 = GetVectorLength(vec1);
		float vecLen2 = GetVectorLength(vec2);

		float rad = std::acos(dot / (vecLen1 * vecLen2));
		return RadiansToDegrees(rad);
	}
	
	float DegreesToRadians(float degrees)
	{
		return degrees * PI / 180.f;
	}

	float RadiansToDegrees(float radians)
	{
		
		return radians * (180.f / PI);

	}

	float LerpFloat(float a, float b, float alpha)
	{
		if (alpha > 1) alpha = 1;
		if (alpha < 0) alpha = 0;

		return a + (b - a) * alpha;
	}

    unsigned char LerpChar(unsigned char a, unsigned char  b, unsigned char  alpha)
    {
        if (alpha > 1) alpha = 1;
        if (alpha < 0) alpha = 0;

        return a + (b - a) * alpha;
    }


	Color LerpColor(const Color& a, const Color& b, float alpha)
	{
		unsigned char   lerpR = LerpChar(a.r, b.r, (unsigned char )alpha);
        unsigned char   lerpG = LerpChar(a.g, b.g, (unsigned char )alpha);
        unsigned char   lerpB = LerpChar(a.b, b.b, (unsigned char )alpha);
        unsigned char   lerpA = LerpChar(a.a, b.a, (unsigned char )alpha);

		return Color{lerpR, lerpG, lerpB, lerpA};
	}

	Vector2 LerpVector(const Vector2& a, const Vector2& b, float alpha)
	{
		float lerpX = LerpFloat(a.x, b.x, alpha);
		float lerpY = LerpFloat(a.y, b.y, alpha);
        Vector2 vec{lerpX, lerpY};
		return vec;
	}

	float RandomRange(float min, float max)
	{
		std::random_device rd;
		std::mt19937 gen(rd());

		std::uniform_real_distribution<float> distribution{ min, max };

		return distribution(gen);
	}

	Vector2 RandomUnitVector()
	{
		float randomX = RandomRange(-1, 1);
		float randomY = RandomRange(-1, 1);
		Vector2 randVec{ randomX, randomY };
        NormailzeVec(randVec);
		return randVec;
	}

	Color RandomColor(Color col1, Color col2)
	{
		float randomR = RandomRange(col1.r, col2.r);
		float randomG= RandomRange(col1.g, col2.g);
		float randomB = RandomRange(col1.b, col2.b);
		float randomA = RandomRange(col1.a, col2.a);
		Color randCol = Color{ (uint8_t)randomR, (uint8_t)randomG, (uint8_t)randomB, (uint8_t)randomA };
		
		return randCol;
	}

    Vector2& ScaleVector(Vector2 &vectorToScale, float amt) {
		vectorToScale.x *=  amt;
		vectorToScale.y *= amt;
        return vectorToScale;
    }

	float LERP(float _a, float _b, float _t)
	{
		return _a + _t * (_b - _a);
	}

	Vector2 LERP(Vector2 _a, Vector2 _b, float _t)
	{

		return { LERP(_a.x, _b.x, _t), LERP(_a.y, _b.y, _t) };
	}

	Vector3 LERP(Vector3 _a, Vector3 _b, float _t)
	{

		return { LERP(_a.x, _b.x, _t), LERP(_a.y, _b.y, _t), LERP(_a.z, _b.z, _t) };
	}
	Color LERP(Color _col1, Color _col2, float _t, bool _alpha)
	{
		float r = LERP(_col1.r, _col2.r, _t);
		float g = LERP(_col1.g, _col2.g, _t);
		float b = LERP(_col1.b, _col2.b, _t);
		float a = _col1.a;
		if (_alpha == true) {
			a = LERP(_col1.a, _col2.a, _t);
		}
		return Color{ (unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a };
	}

    float GetVectorLength(Vector2 vector) {
		float len = std::sqrt(vector.x * vector.x + vector.y * vector.y);
		return std::abs(len);
    }

	float GetRawVectorLength(Vector2 vector) {
		float len = std::sqrt(vector.x * vector.x + vector.y * vector.y);
        return len;
	}


    Vector2 NormailzeVec(Vector2 vector)
    {
        float vectorLength = GetVectorLength(vector);
        if (vectorLength == 0.f) {
            return Vector2{};
        }
        ScaleVector(vector, 1.f/vectorLength);
        return vector;
    }

	Vector2 Direction(const Vector2 &begin, const Vector2 &end) {
		float xdir = end.x - begin.x;
		float ydir = end.y - begin.y;
		Vector2 dir = Vector2{ xdir, ydir };
		return dir;

	}

	float Dot(const Vector2& v1, const Vector2& v2)
	{
		Vector2 vec1 = { v1.x , v1.y};
		Vector2 vec2 = { v2.x , v2.y };
		Vector2 normvec1 = NormailzeVec(vec1);
		Vector2 normvec2 = NormailzeVec(vec2);


		
		float result = normvec1.x * normvec2.x + normvec1.y * normvec2.y;

		return result;
	}



	
}