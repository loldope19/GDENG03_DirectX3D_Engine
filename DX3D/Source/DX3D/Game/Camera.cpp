#include <DX3D/Game/Camera.h>
#include <DX3D/Game/InputManager.h>
#include <IMGUI/imgui.h>
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
        ImGuiIO& io = ImGui::GetIO();
        auto input = InputManager::getInstance();

        Vec3 newPos = m_position;
        float movementSpeed = 5.0f * dt;
        float rotationSpeed = 0.5f * dt;
                
        if (input->isKeyDown('W')) { newPos += m_forward * movementSpeed; }
        if (input->isKeyDown('S')) { newPos -= m_forward * movementSpeed; }
        if (input->isKeyDown('A')) { newPos -= m_right * movementSpeed; }
        if (input->isKeyDown('D')) { newPos += m_right * movementSpeed; }
        if (input->isKeyDown('Q')) { newPos -= m_up * movementSpeed; }
        if (input->isKeyDown('E')) { newPos += m_up * movementSpeed; }

        m_position = newPos;

        if (input->isRightMouseDown() && !ImGui::IsAnyItemActive())
        {
            float deltaX = 0.0f;
            float deltaY = 0.0f;
            input->getMouseDelta(deltaX, deltaY);

            m_yaw += deltaX * rotationSpeed;
            m_pitch -= deltaY * rotationSpeed;
            m_pitch = std::clamp(m_pitch, -1.57f, 1.57f);
        }

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