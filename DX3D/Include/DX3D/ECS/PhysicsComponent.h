#pragma once
#include <DX3D/ECS/AComponent.h>
#include <DX3D/Math/Vec3.h>
#include <reactphysics3d/reactphysics3d.h>

using namespace reactphysics3d;
namespace dx3d
{
    class PhysicsSystem;

    class PhysicsComponent final : public AComponent
    {
    public:
        enum class BodyShape
        {
            Box,
            Sphere,
            StaticPlane
        };
        
        PhysicsComponent(GameObject* owner, PhysicsSystem* system, BodyShape shape,
            const Vec3& shapeArgs, const Vec3& localPosition = { 0.0f, 0.0f, 0.0f });

        virtual ~PhysicsComponent() override;
        void renderInInspector() override;

        void onUpdate(float deltaTime) override;
        RigidBody* getRigidBody() const;

    private:
        PhysicsWorld* m_world = nullptr;
        RigidBody* m_rigidBody = nullptr;
        reactphysics3d::CollisionShape* m_collisionShape = nullptr;
    };
}