#pragma once
#include <DX3D/Math/Matrix4x4.h>
#include <DX3D/Math/Vec3.h>

namespace dx3d
{
	class Camera
	{
	public:
		Camera();

		void update(float dt);
		void setProjection(float fovDegrees, float aspectRatio, float nearZ, float farZ);

		const Matrix4x4& getViewMatrix() const;
		const Matrix4x4& getProjectionMatrix() const;

	private:
		void updateViewMatrix();

		Matrix4x4 m_viewMatrix;
		Matrix4x4 m_projectionMatrix;

		Vec3 m_position = { 8.0f, 5.0f, -12.0f }; // Start a bit further back
		Vec3 m_direction;

		float m_yaw = -0.6f;
		float m_pitch = -0.3f;
		const Vec3 worldUp = { 0, 1, 0 };
	};
}