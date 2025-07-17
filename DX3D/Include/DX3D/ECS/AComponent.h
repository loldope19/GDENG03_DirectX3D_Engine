#pragma once
#pragma once
#include <string>

namespace dx3d
{
    class GameObject;

    enum class ComponentType
    {
        NotSet = -1,
        Script,
        Renderer,
        Input,
        Physics
    };

    class AComponent
    {
    public:
        AComponent(const std::string& name, ComponentType type, GameObject* owner) :
            m_name(name),
            m_type(type),
            m_owner(owner)
        {}

        virtual ~AComponent() = default;
        virtual void onUpdate(float deltaTime) = 0;
        virtual void renderInInspector() {}

        GameObject* getOwner() const { return m_owner; }
        ComponentType getType() const { return m_type; }
        const std::string& getName() const { return m_name; }

    protected:
        GameObject* m_owner;
        ComponentType m_type;
        std::string m_name;
    };
}