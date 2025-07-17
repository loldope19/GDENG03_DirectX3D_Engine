#include <DX3D/ECS/PhysicsComponent.h>
#include <DX3D/ECS/PhysicsSystem.h>
#include <DX3D/Game/GameObject.h>
#include <DX3D/Math/Matrix4x4.h>
#include <IMGUI/imgui.h>
#include <iostream>

using namespace reactphysics3d;
namespace dx3d
{
    PhysicsComponent::PhysicsComponent(GameObject* owner, PhysicsSystem* system, BodyShape shape,
        const Vec3& shapeArgs, const Vec3& localPosition)
        : AComponent("PhysicsComponent", ComponentType::Physics, owner)
    {
        m_world = system->getPhysicsWorld();
        PhysicsCommon& physicsCommon = system->getPhysicsCommon();

        Transform rp3dTransform;
        rp3dTransform.setPosition(Vector3(m_owner->getPosition().x, m_owner->getPosition().y, m_owner->getPosition().z));

        m_rigidBody = m_world->createRigidBody(rp3dTransform);
        m_rigidBody->setType(BodyType::DYNAMIC);

        switch (shape)
        {
            case BodyShape::Box:
                m_collisionShape = physicsCommon.createBoxShape(Vector3(shapeArgs.x / 2.0f, shapeArgs.y / 2.0f, shapeArgs.z / 2.0f));
                break;
            case BodyShape::Sphere:
                m_collisionShape = physicsCommon.createSphereShape(shapeArgs.x);
                break;

            case BodyShape::StaticPlane:
                m_collisionShape = physicsCommon.createBoxShape(Vector3(shapeArgs.x, 0.05f, shapeArgs.z));
                break;
        }

        Transform localTransform;
        localTransform.setPosition(Vector3(localPosition.x, localPosition.y, localPosition.z));
        localTransform.setOrientation(Quaternion::identity());

        m_rigidBody->addCollider(m_collisionShape, Transform::identity());
    }

    PhysicsComponent::~PhysicsComponent()
    {
        if (m_rigidBody)
        {
            m_world->destroyRigidBody(m_rigidBody);
        }
    }

    void PhysicsComponent::renderInInspector()
    {
        bool isStatic = m_rigidBody->getType() == reactphysics3d::BodyType::STATIC;
        if (isStatic)
        {
            ImGui::Text("Body Type: Static");
        }
        else
        {
            ImGui::Text("Body Type: Dynamic");
        }

        float linearDamping = m_rigidBody->getLinearDamping();
        if (ImGui::DragFloat("Linear Damping", &linearDamping, 0.01f, 0.0f, 100.0f))
        {
            m_rigidBody->setLinearDamping(linearDamping);
        }

        float angularDamping = m_rigidBody->getAngularDamping();
        if (ImGui::DragFloat("Angular Damping", &angularDamping, 0.01f, 0.0f, 100.0f))
        {
            m_rigidBody->setAngularDamping(angularDamping);
        }

        reactphysics3d::Material& material = m_rigidBody->getCollider(0)->getMaterial();

        float friction = material.getFrictionCoefficient();
        if (ImGui::DragFloat("Friction", &friction, 0.01f, 0.0f, 1.0f))
        {
            material.setFrictionCoefficient(friction);
        }

        float bounciness = material.getBounciness();
        if (ImGui::DragFloat("Bounciness", &bounciness, 0.01f, 0.0f, 1.0f))
        {
            material.setBounciness(bounciness);
        }
    }

    void PhysicsComponent::onUpdate(float deltaTime)
    {
        const reactphysics3d::Transform& transform = m_rigidBody->getTransform();
        const reactphysics3d::Vector3& position = transform.getPosition();
        const reactphysics3d::Quaternion& orientation = transform.getOrientation();

        Matrix4x4 scaleMatrix = Matrix4x4::createScale(m_owner->getScale());
        reactphysics3d::Matrix3x3 rot3x3 = orientation.getMatrix();
        Matrix4x4 rotationMatrix = {};
        rotationMatrix.m[0][0] = rot3x3[0][0]; rotationMatrix.m[0][1] = rot3x3[0][1]; rotationMatrix.m[0][2] = rot3x3[0][2];
        rotationMatrix.m[1][0] = rot3x3[1][0]; rotationMatrix.m[1][1] = rot3x3[1][1]; rotationMatrix.m[1][2] = rot3x3[1][2];
        rotationMatrix.m[2][0] = rot3x3[2][0]; rotationMatrix.m[2][1] = rot3x3[2][1]; rotationMatrix.m[2][2] = rot3x3[2][2];
        rotationMatrix.m[3][3] = 1.0f;

        Matrix4x4 translationMatrix = Matrix4x4::createTranslation({ position.x, position.y, position.z });
        Matrix4x4 finalWorldMatrix = scaleMatrix * rotationMatrix * translationMatrix;
        m_owner->setWorldMatrix(finalWorldMatrix);
    }

    RigidBody* PhysicsComponent::getRigidBody() const
    {
        return m_rigidBody;
    }
}