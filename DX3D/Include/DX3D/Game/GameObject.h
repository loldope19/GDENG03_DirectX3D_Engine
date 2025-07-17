#pragma once
#include <DX3D/Math/Vec3.h>
#include <DX3D/Math/Vec4.h>
#include <DX3D/Math/Matrix4x4.h>
#include <DX3D/Graphics/GraphicsResource.h>
#include <string>

#include <DX3D/ECS/AComponent.h>
#include <vector>
#include <memory>

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

        virtual void onUpdate(float dt)
        {
            for (const auto& comp : m_components)
            {
                comp->onUpdate(dt);
            }
        }
        virtual void render(class GraphicsEngine* engine) = 0;

        // --- Component Management ---
        template<typename T, typename... TArgs>
        T* addComponent(TArgs&&... args)
        {
            static_assert(std::is_base_of<AComponent, T>::value, "T must derive from AComponent");

            auto newComponent = std::make_unique<T>(this, std::forward<TArgs>(args)...);
            T* componentPtr = newComponent.get();
            m_components.push_back(std::move(newComponent));

            return componentPtr;
        }

        template<typename T>
        T* getComponent()
        {
            static_assert(std::is_base_of<AComponent, T>::value, "T must derive from AComponent");

            for (const auto& comp : m_components)
            {
                if (T* target = dynamic_cast<T*>(comp.get()))
                {
                    return target;
                }
            }
            return nullptr;
        }


        // --- Transform and Color Getters/Setters ---
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

        const std::string& getName() const { return m_name; }
        void setName(const std::string& name) { m_name = name; }

        void setWorldMatrix(const Matrix4x4& matrix) { m_worldMatrix = matrix; }

    public:
        const std::vector<std::unique_ptr<AComponent>>& getComponents() const { return m_components; }

    private:
        std::vector<std::unique_ptr<AComponent>> m_components;

    protected:
        void updateWorldMatrix()
        {
            Matrix4x4 scaleMatrix = Matrix4x4::createScale(m_scale);

            Matrix4x4 rotX = Matrix4x4::createRotationX(m_rotation.x);
            Matrix4x4 rotY = Matrix4x4::createRotationY(m_rotation.y);
            Matrix4x4 rotZ = Matrix4x4::createRotationZ(m_rotation.z);
            Matrix4x4 rotationMatrix = rotZ * rotY * rotX;

            Matrix4x4 translationMatrix = Matrix4x4::createTranslation(m_position);

            m_worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;
        }

        Matrix4x4 m_worldMatrix;
        Vec3 m_position;
        Vec3 m_rotation;
        Vec3 m_scale;

        // For Inspector use
        std::string m_name = "GameObject";
        Vec4 m_color = { 1,1,1,1 };
        bool m_overrideColor = false;

        friend class GraphicsEngine;
    };
}