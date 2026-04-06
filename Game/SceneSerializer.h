#pragma once
#include <string>

namespace dx3d
{
    class GraphicsEngine;

    class SceneSerializer
    {
    public:
        SceneSerializer(GraphicsEngine* engine);

        void serialize(const std::string& filepath);
        void deserialize(const std::string& filepath);

    private:
        GraphicsEngine* m_graphicsEngine;
    };
}