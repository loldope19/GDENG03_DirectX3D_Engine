#pragma once
#include <cmath>

namespace dx3d
{
	struct Vec3
	{
		float x = 0, y = 0, z = 0;

		Vec3() = default;
		Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

		Vec3 operator+(const Vec3& other) const { return { x + other.x, y + other.y, z + other.z }; }
		Vec3 operator-(const Vec3& other) const { return { x - other.x, y - other.y, z - other.z }; }
		Vec3 operator*(float scalar) const { return { x * scalar, y * scalar, z * scalar }; }

		Vec3& operator+=(const Vec3& other) { x += other.x; y += other.y; z += other.z; return *this; }
		Vec3& operator-=(const Vec3& other) { x -= other.x; y -= other.y; z -= other.z; return *this; }

		void normalize() {
			float len = sqrt(x * x + y * y + z * z);
			if (len > 0.0001f) { x /= len; y /= len; z /= len; }
		}
		Vec3 normalized() const {
			Vec3 temp = *this;
			temp.normalize();
			return temp;
		}

		static Vec3 cross(const Vec3& v1, const Vec3& v2) {
			return {
				v1.y * v2.z - v1.z * v2.y,
				v1.z * v2.x - v1.x * v2.z,
				v1.x * v2.y - v1.y * v2.x
			};
		}
	};
}