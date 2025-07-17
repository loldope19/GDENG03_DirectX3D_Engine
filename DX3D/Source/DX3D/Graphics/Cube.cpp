#include <DX3D/Graphics/Cube.h>
#include <DX3D/Graphics/GraphicsEngine.h>
#include <DX3D/Graphics/GraphicsLogUtils.h>
#include <DX3D/Graphics/DeviceContext.h>
#include <DX3D/Graphics/Shader.h>
#include <vector>

namespace dx3d
{
	struct CubeVertex
	{
		float x, y, z;    // Position
		float r, g, b, a; // Color
	};

	Cube::Cube(const GraphicsResourceDesc& gDesc) : GameObject(gDesc)
	{
		m_stride = sizeof(CubeVertex);
		m_offset = 0;
		setScale({ 1.0f, 1.0f, 1.0f });

		std::vector<CubeVertex> vertices = {
			// Front face
			{-0.5f, -0.5f, -0.5f, 1,1,0,1},
			{-0.5f,  0.5f, -0.5f, 0,0.9f,1,1},
			{ 0.5f,  0.5f, -0.5f, 1,0,0,1},
			{ 0.5f, -0.5f, -0.5f, 0,1,0,1},
			// Back face
			{ 0.5f, -0.5f,  0.5f, 0,1,0,1},
			{ 0.5f,  0.5f,  0.5f, 1,0,0,1},
			{-0.5f,  0.5f,  0.5f, 0,0.9f,1,1},
			{-0.5f, -0.5f,  0.5f, 1,1,0,1},
		};

		std::vector<UINT> indices = {
			// front face
			0,1,2, 0,2,3,
			// back face
			4,5,6, 4,6,7,
			// top face
			1,5,2, 1,6,5,
			// bottom face
			0,3,4, 0,4,7,
			// right face
			3,2,5, 3,5,4,
			// left face
			7,6,1, 7,1,0,
		};

		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(CubeVertex) * vertices.size());
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = vertices.data();
		DX3DGraphicsLogThrowOnFail(m_device.CreateBuffer(&bufferDesc, &initData, &m_vertexBuffer), "Failed to create cube vertex buffer");

		D3D11_BUFFER_DESC indexBufferDesc = {};
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = static_cast<UINT>(sizeof(UINT) * indices.size());
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		D3D11_SUBRESOURCE_DATA indexInitData = {};
		indexInitData.pSysMem = indices.data();
		DX3DGraphicsLogThrowOnFail(m_device.CreateBuffer(&indexBufferDesc, &indexInitData, &m_indexBuffer), "Failed to create cube index buffer");

		Shader::ShaderDesc vertexShaderDesc = { { m_logger, m_graphicsDevice, m_device, m_factory }, Shader::Type::Vertex, "main", "vs_5_0" };
		m_vertexShader = std::make_unique<Shader>(vertexShaderDesc);
		m_vertexShader->loadFromFile("DX3D/Source/DX3D/Graphics/Shaders/3DVertexShader.hlsl");

		Shader::ShaderDesc pixelShaderDesc = { { m_logger, m_graphicsDevice, m_device, m_factory }, Shader::Type::Pixel, "main", "ps_5_0" };
		m_pixelShader = std::make_unique<Shader>(pixelShaderDesc);
		m_pixelShader->loadFromFile("DX3D/Source/DX3D/Graphics/Shaders/PixelShader.hlsl");

		D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		DX3DGraphicsLogThrowOnFail(m_device.CreateInputLayout(layoutDesc, 2, m_vertexShader->getByteCode().data(), m_vertexShader->getByteCode().size(), &m_inputLayout), "Failed to create cube input layout");

	}

	void Cube::onUpdate(float dt)
	{
		GameObject::onUpdate(dt);
	}

	void Cube::render(GraphicsEngine* engine)
	{
		engine->updateConstantBuffer(m_worldMatrix, engine->getViewMatrix(), engine->getProjectionMatrix());
		auto context = engine->getDeviceContext()->m_context.Get();

		context->IASetInputLayout(m_inputLayout.Get());
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->VSSetShader(m_vertexShader->getVertexShader(), nullptr, 0);
		context->PSSetShader(m_pixelShader->getPixelShader(), nullptr, 0);

		ID3D11Buffer* vertexBuffers[] = { m_vertexBuffer.Get() };
		context->IASetVertexBuffers(0, 1, vertexBuffers, &m_stride, &m_offset);
		context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		context->DrawIndexed(36, 0, 0);
	}
}