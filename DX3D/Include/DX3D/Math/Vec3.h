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
			if (length > 0.0f) // Avoid division by zero
			{
				x /= length;
				y /= length;
				z /= length;
			}
		}

		static Vec3 cross(const Vec3& v1, const Vec3& v2)
		{
			return {
				v1.y * v2.z - v1.z * v2.y,
				v1.z * v2.x - v1.x * v2.z,
				v1.x * v2.y - v1.y * v2.x
			};
		}

		Vec3 operator+(const Vec3& other) const { return { x + other.x, y + other.y, z + other.z }; }
		Vec3 operator-(const Vec3& other) const { return { x - other.x, y - other.y, z - other.z }; }
		Vec3 operator*(float scalar) const { return { x * scalar, y * scalar, z * scalar }; }


	public:
		f32 x{}, y{}, z{};
	};
}