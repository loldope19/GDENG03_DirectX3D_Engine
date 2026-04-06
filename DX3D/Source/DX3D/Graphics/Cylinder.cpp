#include <DX3D/Graphics/Cylinder.h>
#include <DX3D/Graphics/GraphicsEngine.h>
#include <DX3D/Graphics/DeviceContext.h>
#include <DX3D/Graphics/GraphicsLogUtils.h>
#include <vector>

namespace dx3d
{
	struct CylinderVertex
	{
		Vec3 position;
		Vec3 color;
	};

	Cylinder::Cylinder(const GraphicsResourceDesc& gDesc) : GameObject(gDesc)
	{
		m_stride = sizeof(CylinderVertex);
		m_offset = 0;

		std::vector<CylinderVertex> vertices;
		std::vector<UINT> indices;

		const int segments = 30;
		const float height = 2.0f;
		const float radius = 0.5f;
		const float halfHeight = height / 2.0f;

		// --- Create Rings (Top and Bottom) ---
		for (int i = 0; i <= segments; ++i)
		{
			float angle = i * 2.0f * 3.14159265f / segments;
			float x = radius * cos(angle);
			float z = radius * sin(angle);

			// Top vertex
			vertices.push_back({ {x, halfHeight, z}, {x / radius, 1.0f, z / radius} });
			// Bottom vertex
			vertices.push_back({ {x, -halfHeight, z}, {x / radius, 0.0f, z / radius} });
		}

		// --- Create Wall Indices ---
		for (int i = 0; i < segments; ++i)
		{
			int topCurrent = i * 2;
			int botCurrent = i * 2 + 1;
			int topNext = ((i + 1) % segments) * 2;
			int botNext = ((i + 1) % segments) * 2 + 1;

			indices.push_back(topCurrent);
			indices.push_back(botNext);
			indices.push_back(botCurrent);

			indices.push_back(topCurrent);
			indices.push_back(topNext);
			indices.push_back(botNext);
		}

		// Top Cap
		int topCenterIndex = (int)vertices.size();
		vertices.push_back({ {0, halfHeight, 0}, {0.5f, 1.0f, 0.5f} });
		for (int i = 0; i < segments; ++i)
		{
			indices.push_back(topCenterIndex);
			indices.push_back(i * 2);
			indices.push_back(((i + 1) % segments) * 2);
		}

		// Bottom Cap
		int botCenterIndex = (int)vertices.size();
		vertices.push_back({ {0, -halfHeight, 0}, {0.5f, 0.0f, 0.5f} });
		for (int i = 0; i < segments; ++i)
		{
			indices.push_back(botCenterIndex);
			indices.push_back(((i + 1) % segments) * 2 + 1);
			indices.push_back(i * 2 + 1);
		}

		m_indexCount = (UINT)indices.size();

		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(CylinderVertex) * vertices.size());
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		D3D11_SUBRESOURCE_DATA initData = { vertices.data() };
		DX3DGraphicsLogThrowOnFail(m_device.CreateBuffer(&bufferDesc, &initData, &m_vertexBuffer), "Failed to create cylinder vertex buffer");

		D3D11_BUFFER_DESC indexBufferDesc = {};
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = static_cast<UINT>(sizeof(UINT) * indices.size());
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		D3D11_SUBRESOURCE_DATA indexInitData = { indices.data() };
		DX3DGraphicsLogThrowOnFail(m_device.CreateBuffer(&indexBufferDesc, &indexInitData, &m_indexBuffer), "Failed to create cylinder index buffer");

		Shader::ShaderDesc vsDesc = { { m_logger, m_graphicsDevice, m_device, m_factory }, Shader::Type::Vertex, "main", "vs_5_0" };
		m_vertexShader = std::make_unique<Shader>(vsDesc);
		if (!m_vertexShader->loadFromFile("DX3D/Source/DX3D/Graphics/Shaders/3DVertexShader.hlsl"))
		{
			DX3DLogThrowError("Failed to load/compile vertex shader for Cylinder.");
		}

		Shader::ShaderDesc psDesc = { { m_logger, m_graphicsDevice, m_device, m_factory }, Shader::Type::Pixel, "main", "ps_5_0" };
		m_pixelShader = std::make_unique<Shader>(psDesc);
		if (!m_pixelShader->loadFromFile("DX3D/Source/DX3D/Graphics/Shaders/PixelShader.hlsl"))
		{
			DX3DLogThrowError("Failed to load/compile pixel shader for Cylinder.");
		}

		D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		DX3DGraphicsLogThrowOnFail(m_device.CreateInputLayout(layoutDesc, 2, m_vertexShader->getByteCode().data(), m_vertexShader->getByteCode().size(), &m_inputLayout), "Failed to create cylinder input layout");
	}

	void Cylinder::onUpdate(float dt)
	{
		GameObject::onUpdate(dt);
	}

	void Cylinder::render(GraphicsEngine* engine)
	{
		engine->updateConstantBuffer(m_worldMatrix, engine->getViewMatrix(), engine->getProjectionMatrix());
		auto context = engine->getDeviceContext()->m_context.Get();

		auto cullNoneState = engine->getRasterizerStateCullNone();
		auto cullBackState = engine->getRasterizerStateCullBack();

		context->RSSetState(cullNoneState);
		context->IASetInputLayout(m_inputLayout.Get());
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->VSSetShader(m_vertexShader->getVertexShader(), nullptr, 0);
		context->PSSetShader(m_pixelShader->getPixelShader(), nullptr, 0);

		UINT stride = sizeof(CylinderVertex);
		UINT offset = 0;
		context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->VSSetShader(m_vertexShader->getVertexShader(), nullptr, 0);
		context->PSSetShader(m_pixelShader->getPixelShader(), nullptr, 0);

		ID3D11Buffer* vertexBuffers[] = { m_vertexBuffer.Get() };
		context->IASetVertexBuffers(0, 1, vertexBuffers, &m_stride, &m_offset);
		context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		context->DrawIndexed(m_indexCount, 0, 0);
		context->RSSetState(cullBackState);
	}
}