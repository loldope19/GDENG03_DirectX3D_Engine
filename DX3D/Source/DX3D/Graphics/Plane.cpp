#include <DX3D/Graphics/Plane.h>
#include <DX3D/Graphics/GraphicsEngine.h>
#include <DX3D/Graphics/GraphicsLogUtils.h>
#include <DX3D/Graphics/DeviceContext.h>
#include <DX3D/Graphics/Shader.h>
#include <DX3D/Math/Vec4.h>
#include <vector>

namespace dx3d
{
	struct PlaneVertex
	{
		float x, y, z;
		float r, g, b, a;
	};

	Plane::Plane(const GraphicsResourceDesc& gDesc) : GameObject(gDesc)
	{
		m_stride = sizeof(PlaneVertex);
		m_offset = 0;
		Vec4 color = { 0.4f, 0.4f, 0.4f, 1.0f }; // A default grey color

		std::vector<PlaneVertex> vertices = {
			{-0.5f, 0.0f,  0.5f, color.x, color.y, color.z, color.w},
			{ 0.5f, 0.0f,  0.5f, color.x, color.y, color.z, color.w},
			{ 0.5f, 0.0f, -0.5f, color.x, color.y, color.z, color.w},
			{-0.5f, 0.0f, -0.5f, color.x, color.y, color.z, color.w}
		};

		std::vector<UINT> indices = { 0, 1, 2, 0, 2, 3 };

		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(PlaneVertex) * vertices.size());
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		D3D11_SUBRESOURCE_DATA initData = { vertices.data() };
		DX3DGraphicsLogThrowOnFail(m_device.CreateBuffer(&bufferDesc, &initData, &m_vertexBuffer), "Failed to create plane vertex buffer");

		D3D11_BUFFER_DESC indexBufferDesc = {};
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = static_cast<UINT>(sizeof(UINT) * indices.size());
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		D3D11_SUBRESOURCE_DATA indexInitData = { indices.data() };
		DX3DGraphicsLogThrowOnFail(m_device.CreateBuffer(&indexBufferDesc, &indexInitData, &m_indexBuffer), "Failed to create plane index buffer");

		// The Constant Buffer is no longer created here.

		Shader::ShaderDesc vsDesc = { { m_logger, m_graphicsDevice, m_device, m_factory }, Shader::Type::Vertex, "main", "vs_5_0" };
		m_vertexShader = std::make_unique<Shader>(vsDesc);
		m_vertexShader->loadFromFile("DX3D/Source/DX3D/Graphics/Shaders/3DVertexShader.hlsl");

		Shader::ShaderDesc psDesc = { { m_logger, m_graphicsDevice, m_device, m_factory }, Shader::Type::Pixel, "main", "ps_5_0" };
		m_pixelShader = std::make_unique<Shader>(psDesc);
		m_pixelShader->loadFromFile("DX3D/Source/DX3D/Graphics/Shaders/PixelShader.hlsl");

		D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		DX3DGraphicsLogThrowOnFail(m_device.CreateInputLayout(layoutDesc, 2, m_vertexShader->getByteCode().data(), m_vertexShader->getByteCode().size(), &m_inputLayout), "Failed to create plane input layout");
	}

	void Plane::render(GraphicsEngine* engine)
	{
		engine->updateConstantBuffer(m_worldMatrix, engine->getViewMatrix(), engine->getProjectionMatrix());
		auto context = engine->getDeviceContext()->m_context.Get();

		auto cullNoneState = engine->getRasterizerStateCullNone();
		auto cullBackState = engine->getRasterizerStateCullBack();

		context->RSSetState(cullNoneState);
		context->IASetInputLayout(m_inputLayout.Get());
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		UINT stride = sizeof(PlaneVertex);
		UINT offset = 0;
		context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->VSSetShader(m_vertexShader->getVertexShader(), nullptr, 0);
		context->PSSetShader(m_pixelShader->getPixelShader(), nullptr, 0);

		ID3D11Buffer* vertexBuffers[] = { m_vertexBuffer.Get() };
		context->IASetVertexBuffers(0, 1, vertexBuffers, &m_stride, &m_offset);
		context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		context->DrawIndexed(6, 0, 0);

		context->RSSetState(cullBackState);
	}
}