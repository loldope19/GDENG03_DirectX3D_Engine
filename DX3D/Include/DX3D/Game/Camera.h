#pragma once
#include <DX3D/Math/Matrix4x4.h>
#include <DX3D/Math/Vec3.h>
#include <algorithm>

namespace dx3d
{
    class Camera
    {
    public:
        Camera();
        void update(float dt);

        const Matrix4x4& getViewMatrix() const { return m_viewMatrix; }
        const Matrix4x4& getProjectionMatrix() const { return m_projectionMatrix; }

        void setProjection(float fovDegrees, float aspectRatio, float nearZ, float farZ);

    private:
        void updateViewMatrix();
        void updateVectors();

    private:
        Matrix4x4 m_viewMatrix;
        Matrix4x4 m_projectionMatrix;

        Vec3 m_position = { 0.0f, 3.0f, -10.0f };
        Vec3 m_forward;
        Vec3 m_right;
        Vec3 m_up;
        const Vec3 m_worldUp = { 0.0f, 1.0f, 0.0f };

        float m_yaw = 0.0f;
        float m_pitch = 0.0f;
    };
}