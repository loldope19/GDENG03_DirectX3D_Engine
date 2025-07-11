#include <DX3D/Game/Camera.h>
#include <DX3D/Game/InputManager.h>
#include <cmath>

namespace dx3d
{
	Camera::Camera()
	{
		updateVectors();
		updateViewMatrix();
	}

	void Camera::update(float dt)
	{
		auto input = InputManager::getInstance();
		const float moveSpeed = 10.0f * dt;
		const float rotSpeed = 0.2f * dt;

		if (input->isRightMouseDown())
		{
			float mouseX, mouseY;
			input->getMouseDelta(mouseX, mouseY);

			m_yaw += mouseX * rotSpeed;
			m_pitch -= mouseY * rotSpeed; // Invert for natural feel

			m_pitch = std::max(-1.55f, std::min(1.55f, m_pitch)); // Clamp to ~89 degrees
		}

		if (input->isKeyDown('W')) m_position += m_forward * moveSpeed;
		if (input->isKeyDown('S')) m_position -= m_forward * moveSpeed;
		if (input->isKeyDown('A')) m_position -= m_right * moveSpeed;
		if (input->isKeyDown('D')) m_position += m_right * moveSpeed;
		if (input->isKeyDown('E')) m_position += m_worldUp * moveSpeed;
		if (input->isKeyDown('Q')) m_position -= m_worldUp * moveSpeed;

		updateVectors();
		updateViewMatrix();
	}

	void Camera::updateVectors()
	{
		m_forward.x = sin(m_yaw) * cos(m_pitch);
		m_forward.y = sin(m_pitch);
		m_forward.z = cos(m_yaw) * cos(m_pitch);
		m_forward.normalize();

		m_right = Vec3::cross(m_worldUp, m_forward).normalized();
		m_up = Vec3::cross(m_forward, m_right).normalized();
	}

	void Camera::updateViewMatrix()
	{
		m_viewMatrix = Matrix4x4::createLookAtLH(m_position, m_position + m_forward, m_up);
	}

	void Camera::setProjection(float fovDegrees, float aspectRatio, float nearZ, float farZ)
	{
		const float fovRadians = fovDegrees * 3.14159265f / 180.0f;
		m_projectionMatrix = Matrix4x4::createPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
	}
}