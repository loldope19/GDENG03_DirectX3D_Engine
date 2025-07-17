#pragma once
#include <DX3D/ECS/BaseComponentSystem.h>
#include <reactphysics3d/reactphysics3d.h>
#include <memory>

namespace dx3d
{
    class PhysicsSystem final : public BaseComponentSystem
    {
    public:
        PhysicsSystem();
        virtual ~PhysicsSystem();

        void update(float deltaTime) override;

        reactphysics3d::PhysicsWorld* getPhysicsWorld() const;
        reactphysics3d::PhysicsCommon& getPhysicsCommon();

    private:
        std::unique_ptr<reactphysics3d::PhysicsCommon> m_physicsCommon;
        reactphysics3d::PhysicsWorld* m_physicsWorld = nullptr;
    };
}