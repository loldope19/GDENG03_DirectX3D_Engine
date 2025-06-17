#include <DX3D/Game/Camera.h>
#include <cmath>

namespace dx3d
{
    Camera::Camera(const Vec3& position, float movementSpeed, float rotationSpeed)
        : m_position(position), m_movementSpeed(movementSpeed), m_rotationSpeed(rotationSpeed)
    {
        update(0.0f, false, false, false, false, 0.0f, 0.0f);
    }

    void Camera::update(float dt, bool moveForward, bool moveBackward, bool moveLeft, bool moveRight, float deltaX, float deltaY)
    {
        m_yaw += deltaX * m_rotationSpeed * dt;
        m_pitch -= deltaY * m_rotationSpeed * dt;

        constexpr float maxPitch = 1.55334f; // 89 degrees in radians
        if (m_pitch > maxPitch) m_pitch = maxPitch;
        if (m_pitch < -maxPitch) m_pitch = -maxPitch;

        m_forward.x = cos(m_yaw) * cos(m_pitch);
        m_forward.y = sin(m_pitch);
        m_forward.z = sin(m_yaw) * cos(m_pitch);
        m_forward.normalize();

        Vec3 worldUp = { 0.0f, 1.0f, 0.0f };
        m_right = Vec3::cross(m_forward, worldUp);
        m_right.normalize();

        m_up = Vec3::cross(m_right, m_forward);
        m_up.normalize();

        Vec3 velocity = { 0,0,0 };
        if (moveForward)  velocity = velocity + m_forward;
        if (moveBackward) velocity = velocity - m_forward;
        if (moveLeft)     velocity = velocity + m_right;
        if (moveRight)    velocity = velocity - m_right;

        m_position = m_position + velocity * m_movementSpeed * dt;

        updateViewMatrix();
    }

    Matrix4x4 Camera::getViewMatrix()
    {
        return m_viewMatrix;
    }

    void Camera::updateViewMatrix()
    {
        Vec3 target = m_position + m_forward;
        m_viewMatrix = Matrix4x4::lookAt(m_position, target, m_up);
    }
}