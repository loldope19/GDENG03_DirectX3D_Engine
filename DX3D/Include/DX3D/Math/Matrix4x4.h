#pragma once
#include <DX3D/Math/Vec3.h>
#include <DirectXMath.h>

namespace dx3d
{
	__declspec(align(16)) struct Matrix4x4
	{
		float m[4][4];

		Matrix4x4();

		Matrix4x4 operator*(const Matrix4x4& other) const;

		// Static functions are now the only way to create new matrices
		static Matrix4x4 createTranslation(const Vec3& translation);
		static Matrix4x4 createRotationX(float angle);
		static Matrix4x4 createRotationY(float angle);
		static Matrix4x4 createRotationZ(float angle);
		static Matrix4x4 createScale(const Vec3& scale);
		static Matrix4x4 createPerspectiveFovLH(float fovY, float aspectRatio, float nearPlane, float farPlane);
		static Matrix4x4 createLookAtLH(const Vec3& eye, const Vec3& target, const Vec3& up);
		static Matrix4x4 transpose(const Matrix4x4& matrix);
	};
}