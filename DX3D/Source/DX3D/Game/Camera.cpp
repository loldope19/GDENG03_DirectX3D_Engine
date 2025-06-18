#include <DX3D/Game/Camera.h>
#include <DX3D/Game/InputManager.h>
#include <cmath>

namespace dx3d
{
	Camera::Camera()
	{
		updateViewMatrix();
	}

	void Camera::update(float dt)
	{
		auto input = InputManager::getInstance();
		const float moveSpeed = 200.0f * dt;
		const float rotSpeed = 20.0f * dt;

		const Vec3 worldUp = { 0, 1, 0 };
		const Vec3 right = Vec3::cross(worldUp, m_direction).normalized();

		if (input->isKeyDown('W')) m_position += m_direction * moveSpeed;
		if (input->isKeyDown('S')) m_position -= m_direction * moveSpeed;
		if (input->isKeyDown('A')) m_position -= right * moveSpeed;
		if (input->isKeyDown('D')) m_position += right * moveSpeed;

		if (input->isRightMouseDown())
		{
			float mouseX, mouseY;
			input->getMouseDelta(mouseX, mouseY);

			m_yaw += mouseX * rotSpeed;
			m_pitch += mouseY * rotSpeed;

			m_pitch = std::max(-1.57f, std::min(1.57f, m_pitch));
		}

		updateViewMatrix();
	}

	void Camera::updateViewMatrix()
	{
		Matrix4x4 rotationMatrix;
		rotationMatrix.setRotation(m_pitch, m_yaw, 0);

		m_direction = rotationMatrix.getZDirection();
		m_direction.normalize();

		const Vec3 worldUp = { 0, 1, 0 };

		m_viewMatrix.lookAt(m_position, m_position + m_direction, worldUp);
	}

	void Camera::setProjection(float fovDegrees, float aspectRatio, float nearZ, float farZ)
	{
		const float fovRadians = fovDegrees * 3.14159265f / 180.0f;
		m_projectionMatrix.setPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
	}

	const Matrix4x4& Camera::getViewMatrix() const
	{
		return m_viewMatrix;
	}

	const Matrix4x4& Camera::getProjectionMatrix() const
	{
		return m_projectionMatrix;
	}
}