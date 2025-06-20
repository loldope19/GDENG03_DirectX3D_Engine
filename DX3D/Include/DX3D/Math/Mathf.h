#pragma once
#include <DX3D/Math/Vec3.h>

namespace dx3d
{
	namespace Mathf
	{
		inline Vec3 lerp(const Vec3& start, const Vec3& end, float t)
		{
			t = (t > 1.0f) ? 1.0f : t;
			t = (t < 0.0f) ? 0.0f : t;

			return start * (1.0f - t) + end * t;
		}
	}
}