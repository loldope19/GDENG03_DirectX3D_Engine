#pragma once
#include <DX3D/Math/Vec3.h>
#include <DX3D/Math/Matrix4x4.h>

namespace dx3d
{
    class Camera
    {
    public:
        Camera(const Vec3& position, float movementSpeed, float rotationSpeed);

        void update(float dt, bool moveForward, bool moveBackward, bool moveLeft, bool moveRight, float deltaX, float deltaY);
        Matrix4x4 getViewMatrix();

    private:
        void updateViewMatrix();

        Vec3 m_position;
        float m_movementSpeed;
        float m_rotationSpeed;

        float m_yaw = 1.5707963f; // PI / 2
        float m_pitch = 0.0f;

        Vec3 m_forward = { 0.0f, 0.0f, 1.0f };
        Vec3 m_right = { 1.0f, 0.0f, 0.0f };
        Vec3 m_up = { 0.0f, 1.0f, 0.0f };

        Matrix4x4 m_viewMatrix;
    };
}