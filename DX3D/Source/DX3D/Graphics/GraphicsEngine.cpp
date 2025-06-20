#include <DX3D/Graphics/GraphicsEngine.h>
#include <DX3D/Graphics/GraphicsDevice.h>
#include <DX3D/Graphics/DeviceContext.h>
#include <DX3D/Graphics/SwapChain.h>
#include <DX3D/Graphics/GraphicsPipelineState.h>
#include <DX3D/Graphics/GraphicsLogUtils.h>
#include <DX3D/Game/GameObject.h>
#include <DX3D/Game/InputManager.h>

namespace dx3d
{
	struct ConstantBuffer
	{
		Matrix4x4 world;
		Matrix4x4 view;
		Matrix4x4 projection;
	};

	struct MaterialConstantBuffer
	{
		Vec4 color;
		bool overrideColor;
		float padding[3];
	};

	GraphicsEngine::GraphicsEngine(const GraphicsEngineDesc& desc) : Base(desc.base)
	{
		m_graphicsDevice = std::make_shared<GraphicsDevice>(GraphicsDeviceDesc{ m_logger });
		m_deviceContext = m_graphicsDevice->createDeviceContext();

		m_camera = std::make_unique<Camera>();
		m_camera->setProjection(90.0f, static_cast<float>(desc.windowWidth) / desc.windowHeight, 0.1f, 1000.0f);

		D3D11_BUFFER_DESC constBufferDesc = {};
		constBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constBufferDesc.ByteWidth = sizeof(ConstantBuffer);
		constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		DX3DGraphicsLogThrowOnFail(m_graphicsDevice->m_d3dDevice->CreateBuffer(&constBufferDesc, nullptr, &m_constantBuffer), "Failed to create engine constant buffer");

		D3D11_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		rasterizerDesc.FrontCounterClockwise = FALSE;
		rasterizerDesc.DepthClipEnable = TRUE;
		m_graphicsDevice->m_d3dDevice->CreateRasterizerState(&rasterizerDesc, &m_rasterizerStateCullNone);

		D3D11_RASTERIZER_DESC cullBackDesc = {};
		cullBackDesc.FillMode = D3D11_FILL_SOLID;
		cullBackDesc.CullMode = D3D11_CULL_BACK;
		cullBackDesc.FrontCounterClockwise = FALSE;
		cullBackDesc.DepthClipEnable = TRUE;
		m_graphicsDevice->m_d3dDevice->CreateRasterizerState(&cullBackDesc, &m_rasterizerStateCullBack);

		D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
		m_graphicsDevice->m_d3dDevice->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);

		D3D11_BUFFER_DESC materialConstBufferDesc = {};
		materialConstBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		materialConstBufferDesc.ByteWidth = sizeof(MaterialConstantBuffer);
		materialConstBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		materialConstBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		DX3DGraphicsLogThrowOnFail(m_graphicsDevice->m_d3dDevice->CreateBuffer(&materialConstBufferDesc, nullptr, &m_materialConstantBuffer), "Failed to create material constant buffer");

	}

	GraphicsEngine::~GraphicsEngine() = default;

	void GraphicsEngine::onUpdate(float dt)
	{
		m_camera->update(dt);

		for (auto const& go : m_gameObjects)
		{
			go->update(dt);
		}

		if (m_selectedObject)
		{
			auto input = InputManager::getInstance();
			const float moveSpeed = 5.0f * dt;
			const float rotSpeed = 10000.0f * dt;
			const float scaleSpeed = 1.0f * dt;

			Vec3 currentPos = m_selectedObject->getPosition();
			Vec3 currentRot = m_selectedObject->getRotation();
			Vec3 currentScale = m_selectedObject->getScale();

			// Position Controls (I, K, J, L, U, O)
			if (input->isKeyDown('I')) currentPos.z += moveSpeed;
			if (input->isKeyDown('K')) currentPos.z -= moveSpeed;
			if (input->isKeyDown('J')) currentPos.x -= moveSpeed;
			if (input->isKeyDown('L')) currentPos.x += moveSpeed;
			if (input->isKeyDown('U')) currentPos.y += moveSpeed;
			if (input->isKeyDown('O')) currentPos.y -= moveSpeed;

			m_selectedObject->setPosition(currentPos);

			// Rotation Controls (Arrow Keys)
			if (input->isKeyDown(VK_UP)) currentRot.x -= rotSpeed;
			if (input->isKeyDown(VK_DOWN)) currentRot.x += rotSpeed;
			if (input->isKeyDown(VK_LEFT)) currentRot.y -= rotSpeed;
			if (input->isKeyDown(VK_RIGHT)) currentRot.y += rotSpeed;

			currentRot.x += rotSpeed;
			currentRot.y += rotSpeed;
			currentRot.z += rotSpeed;
			m_selectedObject->setRotation(currentRot);

			// Scale Controls (+/- keys)
			if (input->isKeyDown(VK_OEM_PLUS)) currentScale += Vec3(scaleSpeed, scaleSpeed, scaleSpeed);
			if (input->isKeyDown(VK_OEM_MINUS)) currentScale -= Vec3(scaleSpeed, scaleSpeed, scaleSpeed);

			m_selectedObject->setScale(currentScale);
		}
	}

	void GraphicsEngine::render(SwapChain& swapChain)
	{
		auto& context = *m_deviceContext;
		context.clearAndSetBackBuffer(swapChain, { 0.1f, 0.1f, 0.15f, 1.0f });

		context.m_context->OMSetDepthStencilState(m_depthStencilState.Get(), 0);
		context.m_context->RSSetState(m_rasterizerStateCullNone.Get());

		D3D11_VIEWPORT viewport = {};
		DXGI_SWAP_CHAIN_DESC desc;
		swapChain.m_swapChain->GetDesc(&desc);
		viewport.Width = static_cast<float>(desc.BufferDesc.Width);
		viewport.Height = static_cast<float>(desc.BufferDesc.Height);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		context.m_context->RSSetViewports(1, &viewport);

		context.m_context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
		context.m_context->PSSetConstantBuffers(0, 1, m_materialConstantBuffer.GetAddressOf());


		for (auto const& go : m_gameObjects)
		{
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			context.m_context->Map(m_materialConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			auto materialDataPtr = static_cast<MaterialConstantBuffer*>(mappedResource.pData);
			materialDataPtr->overrideColor = go->m_overrideColor;
			materialDataPtr->color = go->m_color;
			context.m_context->Unmap(m_materialConstantBuffer.Get(), 0);

			go->render(this);
		}

		m_graphicsDevice->executeCommandList(context);
		swapChain.present();
	}

	void GraphicsEngine::addGameObject(std::unique_ptr<GameObject> go)
	{
		m_gameObjects.push_back(std::move(go));
	}

	void GraphicsEngine::selectObject(size_t index)
	{
		if (index < m_gameObjects.size())
		{
			m_selectedObject = m_gameObjects[index].get();
			DX3DLogInfo("Selected object at index");
		}
		else
		{
			m_selectedObject = nullptr;
			DX3DLogInfo("Deselected all objects.");
		}
	}

	void GraphicsEngine::updateConstantBuffer(const Matrix4x4& world, const Matrix4x4& view, const Matrix4x4& projection)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		auto context = m_deviceContext->m_context.Get();
		context->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		auto dataPtr = static_cast<ConstantBuffer*>(mappedResource.pData);

		dataPtr->world = world;
		dataPtr->world.transpose();
		dataPtr->view = view;
		dataPtr->view.transpose();
		dataPtr->projection = projection;
		dataPtr->projection.transpose();

		context->Unmap(m_constantBuffer.Get(), 0);
	}

	Matrix4x4 GraphicsEngine::getViewMatrix() const { return m_camera->getViewMatrix(); }
	Matrix4x4 GraphicsEngine::getProjectionMatrix() const { return m_camera->getProjectionMatrix(); }

	GraphicsDevice& GraphicsEngine::getGraphicsDevice() const
	{
		return *m_graphicsDevice;
	}

	GraphicsResourceDesc GraphicsEngine::getGraphicsResourceDesc() const
	{
		return {
			{m_logger},
			m_graphicsDevice,
			*m_graphicsDevice->m_d3dDevice.Get(),
			*m_graphicsDevice->m_dxgiFactory.Get()
		};
	}
}