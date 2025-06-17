#pragma once
#include <DX3D/Core/Core.h>
#include <DX3D/Core/Base.h>
#include <DX3D/Graphics/Triangle.h> 
#include <DX3D/Graphics/Rectangle.h>
#include <DX3D/Graphics/Cube.h>
#include <DX3D/Graphics/AnimatedRectangle.h>
#include <DX3D/Graphics/Plane.h>
#include <DX3D/Game/Camera.h>
#include <DX3D/Math/Vec4.h>
#include <DX3D/Math/Matrix4x4.h>
#include <array>
#include <vector>

namespace dx3d
{
    struct VertexState {
        float x, y, z;
        Vec4 color;
    };

    class GraphicsEngine final : public Base
    {
    public:
        explicit GraphicsEngine(const GraphicsEngineDesc& desc);
        virtual ~GraphicsEngine() override;

        GraphicsDevice& getGraphicsDevice() noexcept;

        void render(SwapChain& swapChain);

        // add a triangle at specified position with specified color
        void addTriangle(float posX, float posY, float size = 1.0f,
            float r = -1.0f, float g = -1.0f, float b = -1.0f, float a = 1.0f);

        // add a rectangle at specified position with specified size and color
        void addRectangle(float posX, float posY, float width = 1.0f, float height = 1.0f,
            float r = -1.0f, float g = -1.0f, float b = -1.0f, float a = 1.0f);

        // add a cube at specified position with specified size and color
        void createCube(const Vec3& position, const Vec3& scale, const Vec3& rotationAxis, float rotationSpeed);

        void addAnimatedRectangle(
            const std::array<VertexState, 4>& state_A_vertices,
            const std::array<VertexState, 4>& state_B_vertices
        );

        static void toggleRotation();
        static bool isRotationEnabled();

        void createPlane(const Vec3& position, const Vec3& scale, const Vec4& color);
        void onUpdate(float dt, bool moveForward, bool moveBackward, bool moveLeft, bool moveRight, float deltaX, float deltaY);


    private:
        std::shared_ptr<GraphicsDevice> m_graphicsDevice{};
        DeviceContextPtr m_deviceContext{};
        GraphicsPipelineStatePtr m_pipeline{};
        Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerStateCullNone{};

        std::unique_ptr<Triangle> m_triangleManager{};
        std::unique_ptr<Rectangle> m_rectangleManager{};
        std::vector<std::unique_ptr<Cube>> m_cubes;
        std::unique_ptr<AnimatedRectangle> m_animatedRectangleManager{};
        std::vector<std::unique_ptr<Plane>> m_planes;
        std::unique_ptr<Camera> m_camera;

        static bool s_rotationEnabled;

        Matrix4x4 m_viewMatrix;
        Matrix4x4 m_projectionMatrix;
    };
}