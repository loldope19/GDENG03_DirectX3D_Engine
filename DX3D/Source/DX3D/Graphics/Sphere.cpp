#include <DX3D/Graphics/Sphere.h>
#include <DX3D/Graphics/GraphicsEngine.h>
#include <DX3D/Graphics/DeviceContext.h>
#include <DX3D/Graphics/GraphicsLogUtils.h>
#include <vector>

namespace dx3d
{
	struct SphereVertex
	{
		Vec3 position;
		Vec3 color;
	};

	Sphere::Sphere(const GraphicsResourceDesc& gDesc) : GameObject(gDesc)
	{
		m_stride = sizeof(SphereVertex);
		m_offset = 0;

		std::vector<SphereVertex> vertices;
		std::vector<UINT> indices;

		// --- Sphere Generation Logic ---
		int latitudeBands = 30;
		int longitudeBands = 30;
		float radius = 1.0f;

		for (int lat = 0; lat <= latitudeBands; ++lat)
		{
			float theta = lat * 3.14159265f / latitudeBands;
			float sinTheta = sin(theta);
			float cosTheta = cos(theta);

			for (int lon = 0; lon <= longitudeBands; ++lon)
			{
				float phi = lon * 2 * 3.14159265f / longitudeBands;
				float sinPhi = sin(phi);
				float cosPhi = cos(phi);

				SphereVertex vs;
				vs.position.x = radius * cosPhi * sinTheta;
				vs.position.y = radius * cosTheta;
				vs.position.z = radius * sinPhi * sinTheta;

				vs.color.x = (vs.position.x + radius) / (2 * radius);
				vs.color.y = (vs.position.y + radius) / (2 * radius);
				vs.color.z = (vs.position.z + radius) / (2 * radius);
				vertices.push_back(vs);
			}
		}

		for (int lat = 0; lat < latitudeBands; ++lat)
		{
			for (int lon = 0; lon < longitudeBands; ++lon)
			{
				int first = (lat * (longitudeBands + 1)) + lon;
				int second = first + longitudeBands + 1;

				indices.push_back(first);
				indices.push_back(second);
				indices.push_back(first + 1);

				indices.push_back(second);
				indices.push_back(second + 1);
				indices.push_back(first + 1);
			}
		}

		m_indexCount = (UINT)indices.size();

		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(SphereVertex) * vertices.size());
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		D3D11_SUBRESOURCE_DATA initData = { vertices.data() };
		DX3DGraphicsLogThrowOnFail(m_device.CreateBuffer(&bufferDesc, &initData, &m_vertexBuffer), "Failed to create sphere vertex buffer");

		D3D11_BUFFER_DESC indexBufferDesc = {};
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = static_cast<UINT>(sizeof(UINT) * indices.size());
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		D3D11_SUBRESOURCE_DATA indexInitData = { indices.data() };
		DX3DGraphicsLogThrowOnFail(m_device.CreateBuffer(&indexBufferDesc, &indexInitData, &m_indexBuffer), "Failed to create sphere index buffer");

		Shader::ShaderDesc vsDesc = { { m_logger, m_graphicsDevice, m_device, m_factory }, Shader::Type::Vertex, "main", "vs_5_0" };
		m_vertexShader = std::make_unique<Shader>(vsDesc);
		m_vertexShader->loadFromFile("DX3D/Source/DX3D/Graphics/Shaders/3DVertexShader.hlsl");

		Shader::ShaderDesc psDesc = { { m_logger, m_graphicsDevice, m_device, m_factory }, Shader::Type::Pixel, "main", "ps_5_0" };
		m_pixelShader = std::make_unique<Shader>(psDesc);
		m_pixelShader->loadFromFile("DX3D/Source/DX3D/Graphics/Shaders/PixelShader.hlsl");

		D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		DX3DGraphicsLogThrowOnFail(m_device.CreateInputLayout(layoutDesc, 2, m_vertexShader->getByteCode().data(), m_vertexShader->getByteCode().size(), &m_inputLayout), "Failed to create sphere input layout");
	}

	void Sphere::update(float dt)
	{
	}

	void Sphere::render(GraphicsEngine* engine)
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

		UINT stride = sizeof(SphereVertex);
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