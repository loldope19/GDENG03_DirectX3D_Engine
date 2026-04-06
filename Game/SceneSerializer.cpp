#include "SceneSerializer.h"
#include <DX3D/Graphics/GraphicsEngine.h>
#include <DX3D/Game/GameObject.h>
#include <DX3D/ECS/PhysicsComponent.h>
#include <DX3D/Graphics/Cube.h>
#include <DX3D/Graphics/Plane.h>
#include <DX3D/Graphics/Sphere.h>
#include <DX3D/Graphics/Capsule.h>
#include <DX3D/Graphics/Cylinder.h>
#include <DX3D/Core/Base.h>
#include <../Vendor/nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

namespace dx3d
{
    // Helper functions to convert Vec3 and Vec4 to/from JSON
    static void to_json(json& j, const Vec3& v) {
        j = json{ {"x", v.x}, {"y", v.y}, {"z", v.z} };
    }

    static void from_json(const json& j, Vec3& v) {
        j.at("x").get_to(v.x);
        j.at("y").get_to(v.y);
        j.at("z").get_to(v.z);
    }

    static void to_json(json& j, const Vec4& v) {
        j = json{ {"x", v.x}, {"y", v.y}, {"z", v.z}, {"w", v.w} };
    }

    static void from_json(const json& j, Vec4& v) {
        j.at("x").get_to(v.x);
        j.at("y").get_to(v.y);
        j.at("z").get_to(v.z);
        j.at("w").get_to(v.w);
    }

    SceneSerializer::SceneSerializer(GraphicsEngine* engine)
        : m_graphicsEngine(engine)
    {
    }

    void SceneSerializer::serialize(const std::string& filepath)
    {
        json sceneJson;
        json gameObjectsJson = json::array();

        // This loop is now safe because m_graphicsEngine is a valid pointer
        for (const auto& go : m_graphicsEngine->getGameObjects())
        {
            json goJson;
            goJson["name"] = go->getName();
            goJson["type"] = go->getPrimitiveType();
            goJson["position"] = go->getPosition();
            goJson["rotation"] = go->getRotation();
            goJson["scale"] = go->getScale();
            goJson["color"] = go->m_color;
            goJson["overrideColor"] = go->m_overrideColor;

            auto physicsComponent = go->getComponent<PhysicsComponent>();
            if (physicsComponent)
            {
                json physicsJson;
                auto rigidBody = physicsComponent->getRigidBody();
                physicsJson["hasRigidbody"] = true;
                physicsJson["isStatic"] = rigidBody->getType() == reactphysics3d::BodyType::STATIC;
                physicsJson["linearDamping"] = rigidBody->getLinearDamping();
                physicsJson["angularDamping"] = rigidBody->getAngularDamping();
                physicsJson["friction"] = rigidBody->getCollider(0)->getMaterial().getFrictionCoefficient();
                physicsJson["bounciness"] = rigidBody->getCollider(0)->getMaterial().getBounciness();
                goJson["physics"] = physicsJson;
            }
            else
            {
                goJson["physics"]["hasRigidbody"] = false;
            }

            gameObjectsJson.push_back(goJson);
        }

        sceneJson["gameObjects"] = gameObjectsJson;

        std::ofstream o(filepath);
        o << std::setw(4) << sceneJson << std::endl;
    }

    void SceneSerializer::deserialize(const std::string& filepath)
    {
        DX3DLog(m_graphicsEngine->getLogger(), Logger::LogLevel::Info, "Attempting to deserialize: " + filepath);

        std::ifstream i(filepath);
        if (!i.is_open())
        {
            DX3DLog(m_graphicsEngine->getLogger(), Logger::LogLevel::Error, "Failed to open level file: " + filepath);
            DX3DLog(m_graphicsEngine->getLogger(), Logger::LogLevel::Error, "Current working directory check needed");
            return;
        }

        DX3DLog(m_graphicsEngine->getLogger(), Logger::LogLevel::Info, "File opened successfully: " + filepath);

        json sceneJson;
        try {
            i >> sceneJson;
            DX3DLog(m_graphicsEngine->getLogger(), Logger::LogLevel::Info, "JSON parsed successfully");
        }
        catch (const std::exception& e) {
            DX3DLog(m_graphicsEngine->getLogger(), Logger::LogLevel::Error, "JSON parse error: " + std::string(e.what()));
            return;
        }

        m_graphicsEngine->clearScene();
        Game* game = m_graphicsEngine->getGame();

        DX3DLog(m_graphicsEngine->getLogger(), Logger::LogLevel::Info, "Deserializing scene from: " + filepath);

        for (const auto& goJson : sceneJson["gameObjects"])
        {
            if (!goJson.contains("type") || !goJson.at("type").is_string())
            {
                DX3DLog(m_graphicsEngine->getLogger(), Logger::LogLevel::Error, "GameObject entry in level file is missing 'type' or it's not a string. Skipping.");
                continue;
            }

            std::string type = goJson.at("type").get<std::string>();
            DX3DLog(m_graphicsEngine->getLogger(), Logger::LogLevel::Info, "Deserializing object of type: " + type);

            std::unique_ptr<GameObject> newGo = nullptr;

            if (type == "Cube")
                newGo = std::make_unique<Cube>(m_graphicsEngine->getGraphicsResourceDesc());
            else if (type == "Plane")
                newGo = std::make_unique<Plane>(m_graphicsEngine->getGraphicsResourceDesc());
            else if (type == "Sphere")
                newGo = std::make_unique<Sphere>(m_graphicsEngine->getGraphicsResourceDesc());
            else if (type == "Capsule")
                newGo = std::make_unique<Capsule>(m_graphicsEngine->getGraphicsResourceDesc());
            else if (type == "Cylinder")
                newGo = std::make_unique<Cylinder>(m_graphicsEngine->getGraphicsResourceDesc());

            if (newGo)
            {
                newGo->setName(goJson.at("name").get<std::string>());
                newGo->setPosition(goJson.at("position").get<Vec3>());
                newGo->setRotation(goJson.at("rotation").get<Vec3>());
                newGo->setScale(goJson.at("scale").get<Vec3>());
                newGo->m_color = goJson.at("color").get<Vec4>();
                newGo->m_overrideColor = goJson.at("overrideColor").get<bool>();

                if (goJson.contains("physics") && goJson.at("physics").at("hasRigidbody").get<bool>())
                {
                    // Now correctly gets the physics system via the game pointer
                    auto physicsComponent = newGo->addComponent<PhysicsComponent>(
                        game->getPhysicsSystem(),
                        PhysicsComponent::BodyShape::Box, // Note: This assumes Box shape. A more robust solution would serialize the shape type.
                        newGo->getScale()
                    );

                    auto rigidBody = physicsComponent->getRigidBody();
                    if (goJson.at("physics").at("isStatic").get<bool>())
                        rigidBody->setType(reactphysics3d::BodyType::STATIC);

                    rigidBody->setLinearDamping(goJson.at("physics").at("linearDamping").get<float>());
                    rigidBody->setAngularDamping(goJson.at("physics").at("angularDamping").get<float>());
                    rigidBody->getCollider(0)->getMaterial().setFrictionCoefficient(goJson.at("physics").at("friction").get<float>());
                    rigidBody->getCollider(0)->getMaterial().setBounciness(goJson.at("physics").at("bounciness").get<float>());
                }

                m_graphicsEngine->addGameObject(std::move(newGo));
            }
            else
            {
                DX3DLog(m_graphicsEngine->getLogger(), Logger::LogLevel::Warning, "Unknown object type in level file: " + type);
            }
        }
        DX3DLog(m_graphicsEngine->getLogger(), Logger::LogLevel::Info, "Finished deserializing scene.");
    }
}