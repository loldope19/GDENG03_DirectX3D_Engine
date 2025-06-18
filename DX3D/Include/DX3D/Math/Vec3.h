#pragma once
#include <DX3D/Core/Core.h>
#include <cmath>

namespace dx3d
{
	class Vec3
	{
	public:
		Vec3() = default;
		Vec3(f32 x, f32 y, f32 z) : x(x), y(y), z(z) {}

		void normalize()
		{
			const f32 length = std::sqrt(x * x + y * y + z * z);
			if (length > 0.0f)
			{
				x /= length;
				y /= length;
				z /= length;
			}
		}

		Vec3 normalized() const
		{
			const f32 length = std::sqrt(x * x + y * y + z * z);
			if (length > 0.0f)
				return { x / length, y / length, z / length };
			return { 0.0f, 0.0f, 0.0f };
		}

		static Vec3 cross(const Vec3& v1, const Vec3& v2)
		{
			return {
				v1.y * v2.z - v1.z * v2.y,
				v1.z * v2.x - v1.x * v2.z,
				v1.x * v2.y - v1.y * v2.x
			};
		}

		static f32 dot(const Vec3& v1, const Vec3& v2)
		{
			return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
		}

		Vec3 operator+(const Vec3& other) const { return { x + other.x, y + other.y, z + other.z }; }
		Vec3& operator+=(const Vec3& other) { x += other.x; y += other.y; z += other.z; return *this; }

		Vec3 operator-() const { return { -x, -y, -z }; }

		Vec3 operator-(const Vec3& other) const { return { x - other.x, y - other.y, z - other.z }; }
		Vec3& operator-=(const Vec3& other) { x -= other.x; y -= other.y; z -= other.z; return *this; }

		Vec3 operator*(f32 scalar) const { return { x * scalar, y * scalar, z * scalar }; }
		Vec3& operator*=(f32 scalar) { x *= scalar; y *= scalar; z *= scalar; return *this; }

	public:
		f32 x{}, y{}, z{};
	};
}
