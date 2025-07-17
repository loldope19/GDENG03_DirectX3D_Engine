#pragma once

namespace dx3d
{
    class BaseComponentSystem
    {
    public:
        virtual ~BaseComponentSystem() = default;
        virtual void update(float deltaTime) = 0;
    };
}