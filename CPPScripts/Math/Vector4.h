#pragma once
#include <string>

namespace ZXEngine
{
	class Vector2;
	class Vector3;
	class Matrix4;
	class Vector4
	{
	public:
		float x;
		float y;
		float z;
		float w;

		Vector4();
		Vector4(float n);
		Vector4(float x, float y, float z, float w);
		Vector4(const Vector2& v2);
		Vector4(const Vector2& v2l, const Vector2& v2r);
		Vector4(const Vector2& v2, float z, float w);
		Vector4(const Vector3& v3);
		Vector4(const Vector3& v3, float w);
		Vector4(const Vector4& v);

		std::string ToString();

		Vector4 operator+ (const Vector4& v) const;
		Vector4 operator- (const Vector4& v) const;
		Vector4 operator* (const Vector4& v) const;
		Vector4 operator/ (const Vector4& v) const;
		Vector4 operator+ (float n) const;
		Vector4 operator- (float n) const;
		Vector4 operator* (float n) const;
		Vector4 operator/ (float n) const;
		Vector4 operator* (const Matrix4& mat) const;
	};
}