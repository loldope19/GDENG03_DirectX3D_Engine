#pragma once
#include <DX3D/Math/Vec3.h>
#include <DX3D/Math/Vec4.h>
#include <DX3D/Math/Matrix4x4.h>
#include <DX3D/Graphics/GraphicsResource.h>

namespace dx3d
{
    class GameObject : public GraphicsResource
    {
    public:
        GameObject(const GraphicsResourceDesc& gDesc)
            : GraphicsResource(gDesc), m_position(0, 0, 0), m_scale(1, 1, 1), m_rotation(0, 0, 0)
        {
            updateWorldMatrix();
        }
        virtual ~GameObject() = default;

        // common virtual methods for all game objects
        virtual void update(float dt) {}
        virtual void render(class GraphicsEngine* engine) = 0;

        void setColor(const Vec3& color) { m_color = { color.x, color.y, color.z, 1.0f }; m_overrideColor = true; }
        void setColor(const Vec4& color) { m_color = color; m_overrideColor = true; }
        void resetColor() { m_overrideColor = false; }

        void setPosition(const Vec3& pos) { m_position = pos; updateWorldMatrix(); }
        void setScale(const Vec3& scale) { m_scale = scale; updateWorldMatrix(); }
        void setRotation(const Vec3& rot) { m_rotation = rot; updateWorldMatrix(); }

        Vec3 getPosition() const { return m_position; }
        Vec3 getScale() const { return m_scale; }
        Vec3 getRotation() const { return m_rotation; }

        Matrix4x4 getWorldMatrix() const { return m_worldMatrix; }

    protected:
        void updateWorldMatrix()
        {
            Matrix4x4 scaleMatrix, rotationMatrix, translationMatrix;
            scaleMatrix.setScale(m_scale);

            Matrix4x4 rotX, rotY, rotZ;
            rotX.setRotationX(m_rotation.x);
            rotY.setRotationY(m_rotation.y);
            rotZ.setRotationZ(m_rotation.z);
            rotationMatrix = rotZ * rotY * rotX;

            translationMatrix.setTranslation(m_position);
            m_worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;
        }

        Matrix4x4 m_worldMatrix;
        Vec3 m_position;
        Vec3 m_scale;
        Vec3 m_rotation;

    public:
        Vec4 m_color;
        bool m_overrideColor;
    };
}