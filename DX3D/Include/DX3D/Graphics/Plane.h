#pragma once

#include <DX3D/Graphics/GraphicsResource.h>
#include <DX3D/Graphics/Shader.h>
#include <DX3D/Math/Matrix4x4.h>
#include <DX3D/Math/Vec4.h>
#include <DX3D/Graphics/Cube.h> // Reusing constant buffer from Cube
#include <memory>
#include <vector>

namespace dx3d
{
    struct PlaneVertex
    {
        float x, y, z;    // Position
        float r, g, b, a; // Color
    };

    class Plane : public GraphicsResource
    {
    public:
        Plane(const GraphicsResourceDesc& gDesc, const Vec3& position, const Vec3& scale, const Vec4& color);

        void render(ID3D11DeviceContext& context, const Matrix4x4& viewMatrix, const Matrix4x4& projectionMatrix);

    private:
        void updateWorldMatrix();

    private:
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;
        Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
        std::unique_ptr<Shader> m_vertexShader;
        std::unique_ptr<Shader> m_pixelShader;

        Matrix4x4 m_worldMatrix;
        Vec3 m_position;
        Vec3 m_scale;

        UINT m_stride;
        UINT m_offset;
    };
}