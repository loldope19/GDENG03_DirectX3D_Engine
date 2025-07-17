#include <DX3D/ECS/PhysicsSystem.h>

using namespace reactphysics3d;
namespace dx3d
{
    PhysicsSystem::PhysicsSystem()
    {
        m_physicsCommon = std::make_unique<reactphysics3d::PhysicsCommon>();

        reactphysics3d::PhysicsWorld::WorldSettings worldSettings;

        worldSettings.gravity = reactphysics3d::Vector3(0, (decimal) - 9.81, 0);
        // set other world properties here, like worldSettings.isSleepingEnabled = true;

        m_physicsWorld = m_physicsCommon->createPhysicsWorld(worldSettings);
    }

    PhysicsSystem::~PhysicsSystem()
    {
        if (m_physicsWorld)
        {
            m_physicsCommon->destroyPhysicsWorld(m_physicsWorld);
        }
    }

    void PhysicsSystem::update(float deltaTime)
    {
        m_physicsWorld->update(deltaTime);
    }

    reactphysics3d::PhysicsWorld* PhysicsSystem::getPhysicsWorld() const
    {
        return m_physicsWorld;
    }

    reactphysics3d::PhysicsCommon& PhysicsSystem::getPhysicsCommon()
    {
        return *m_physicsCommon;
    }
}