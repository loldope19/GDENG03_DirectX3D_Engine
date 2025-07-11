#include <DX3D/Graphics/GraphicsEngine.h>
#include <DX3D/Graphics/GraphicsDevice.h>
#include <DX3D/Graphics/DeviceContext.h>
#include <DX3D/Graphics/SwapChain.h>
#include <DX3D/Graphics/GraphicsPipelineState.h>
#include <DX3D/Graphics/GraphicsLogUtils.h>
#include <DX3D/Game/GameObject.h>
#include <DX3D/Game/InputManager.h>
#include <DX3D/Core/EngineTime.h>

#include <DX3D/Graphics/Cube.h>
#include <DX3D/Graphics/Plane.h>
#include <DX3D/Graphics/Sphere.h>
#include <DX3D/Graphics/Capsule.h>
#include <DX3D/Graphics/Cylinder.h>

#include <IMGUI/imgui.h>
#include <IMGUI/imgui_impl_win32.h>
#include <IMGUI/imgui_impl_dx11.h>
#include <IMGUI/imgui_internal.h>

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

		recreateSceneResources(800, 600);

		D3D11_RASTERIZER_DESC cullBackDesc = {};
		cullBackDesc.FillMode = D3D11_FILL_SOLID;
		cullBackDesc.CullMode = D3D11_CULL_BACK;
		cullBackDesc.FrontCounterClockwise = FALSE;
		cullBackDesc.DepthClipEnable = TRUE;
		m_graphicsDevice->m_d3dDevice->CreateRasterizerState(&cullBackDesc, &m_rasterizerStateCullBack);

		D3D11_RASTERIZER_DESC cullNoneDesc = {};
		cullNoneDesc.FillMode = D3D11_FILL_SOLID;
		cullNoneDesc.CullMode = D3D11_CULL_NONE;
		cullNoneDesc.FrontCounterClockwise = FALSE;
		cullNoneDesc.DepthClipEnable = TRUE;
		m_graphicsDevice->m_d3dDevice->CreateRasterizerState(&cullNoneDesc, &m_rasterizerStateCullNone);

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

	}

	void GraphicsEngine::render(SwapChain& swapChain)
	{
		auto& context = *m_deviceContext;

		D3D11_VIEWPORT viewport = {};
		viewport.Width = m_sceneViewSize.x;
		viewport.Height = m_sceneViewSize.y;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		context.m_context->RSSetViewports(1, &viewport);

		context.m_context->OMSetRenderTargets(1, m_sceneRenderTargetView.GetAddressOf(), m_sceneDepthStencilView.Get());

		float clearColor[] = { 0.1f, 0.1f, 0.15f, 1.0f };
		context.m_context->ClearRenderTargetView(m_sceneRenderTargetView.Get(), clearColor);
		context.m_context->ClearDepthStencilView(m_sceneDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		context.m_context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
		context.m_context->PSSetConstantBuffers(0, 1, m_materialConstantBuffer.GetAddressOf());
		context.m_context->OMSetDepthStencilState(m_depthStencilState.Get(), 0);
		context.m_context->RSSetState(m_rasterizerStateCullBack.Get()); // Use CullBack for 3D objects

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

		context.clearAndSetBackBuffer(swapChain, { 0.0f, 0.0f, 0.0f, 1.0f });

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		renderEditorUI();
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

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

	void GraphicsEngine::removeGameObject(GameObject* object)
	{
		if (!object) return;

		if (m_selectedObject == object)
		{
			m_selectedObject = nullptr;
		}

		m_gameObjects.erase(
			std::remove_if(m_gameObjects.begin(), m_gameObjects.end(),
				[object](const std::unique_ptr<GameObject>& go) {
					return go.get() == object;
				}),
			m_gameObjects.end());
	}

	void GraphicsEngine::updateConstantBuffer(const Matrix4x4& world, const Matrix4x4& view, const Matrix4x4& projection)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		auto context = m_deviceContext->m_context.Get();
		context->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		auto dataPtr = static_cast<ConstantBuffer*>(mappedResource.pData);

		dataPtr->world = Matrix4x4::transpose(world);
		dataPtr->view = Matrix4x4::transpose(view);
		dataPtr->projection = Matrix4x4::transpose(projection);

		context->Unmap(m_constantBuffer.Get(), 0);
	}

	void GraphicsEngine::recreateSceneResources(UINT width, UINT height)
	{
		m_sceneRenderTargetView.Reset();
		m_sceneDepthStencilView.Reset();
		m_sceneShaderResourceView.Reset();

		if (width == 0 || height == 0) return;

		// --- Create Texture ---
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> sceneTexture;
		DX3DGraphicsLogThrowOnFail(m_graphicsDevice->m_d3dDevice->CreateTexture2D(&textureDesc, nullptr, &sceneTexture), "Failed to create scene texture");

		DX3DGraphicsLogThrowOnFail(m_graphicsDevice->m_d3dDevice->CreateRenderTargetView(sceneTexture.Get(), nullptr, &m_sceneRenderTargetView), "Failed to create scene RTV");

		DX3DGraphicsLogThrowOnFail(m_graphicsDevice->m_d3dDevice->CreateShaderResourceView(sceneTexture.Get(), nullptr, &m_sceneShaderResourceView), "Failed to create scene SRV");

		D3D11_TEXTURE2D_DESC depthStencilDesc = {};
		depthStencilDesc.Width = width;
		depthStencilDesc.Height = height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
		DX3DGraphicsLogThrowOnFail(m_graphicsDevice->m_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencilBuffer), "Failed to create scene depth stencil buffer");
		DX3DGraphicsLogThrowOnFail(m_graphicsDevice->m_d3dDevice->CreateDepthStencilView(depthStencilBuffer.Get(), nullptr, &m_sceneDepthStencilView), "Failed to create scene DSV");

		m_sceneViewSize = ImVec2((float)width, (float)height);
	}

	void GraphicsEngine::renderEditorUI()
	{
		{
			double deltaTime = EngineTime::getDeltaTime();
			m_fpsUpdateTimer += static_cast<float>(deltaTime);
			m_frameCount++;

			if (m_fpsUpdateTimer >= FPS_UPDATE_INTERVAL)
			{
				m_lastFPS = static_cast<float>(m_frameCount) / m_fpsUpdateTimer;
				m_frameCount = 0;
				m_fpsUpdateTimer = 0.0f;
			}

			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImVec2 window_pos = ImVec2(viewport->WorkPos.x + 10.0f, viewport->WorkPos.y + viewport->WorkSize.y - 10.0f);
			ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, ImVec2(0.0f, 1.0f));
			ImGui::SetNextWindowBgAlpha(0.35f);
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;

			if (ImGui::Begin("FPS Counter", nullptr, flags))
			{
				ImGui::Text("%.1f FPS (%.2f ms)", m_lastFPS, deltaTime * 1000.0);
			}
			ImGui::End();
		}

		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("MainDockspaceWindow", nullptr, window_flags);
		ImGui::PopStyleVar(3);

		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

		renderSceneView();
		renderMenuBar();
		renderSceneHierarchy();
		renderInspector();

		ImGui::End();
	}

	void GraphicsEngine::renderMenuBar()
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit"))
				{
					// In a real app, you'd post a quit message.
					// For now, we can just log it.
					DX3DLogInfo("Exit menu item clicked.");
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Game Object"))
			{
				if (ImGui::MenuItem("Create Empty")) {}
				ImGui::Separator();
				if (ImGui::MenuItem("Cube"))
				{
					auto cube = std::make_unique<Cube>(getGraphicsResourceDesc());
					cube->setName("Cube");
					addGameObject(std::move(cube));
				}
				if (ImGui::MenuItem("Plane"))
				{
					auto plane = std::make_unique<Plane>(getGraphicsResourceDesc());
					plane->setName("Plane");
					addGameObject(std::move(plane));
				}
				if (ImGui::MenuItem("Sphere"))
				{
					auto sphere = std::make_unique<Sphere>(getGraphicsResourceDesc());
					sphere->setName("Sphere");
					addGameObject(std::move(sphere));
				}
				if (ImGui::MenuItem("Capsule"))
				{
					auto capsule = std::make_unique<Capsule>(getGraphicsResourceDesc());
					capsule->setName("Capsule");
					addGameObject(std::move(capsule));
				}
				if (ImGui::MenuItem("Cylinder"))
				{
					auto cylinder = std::make_unique<Cylinder>(getGraphicsResourceDesc());
					cylinder->setName("Cylinder");
					addGameObject(std::move(cylinder));
				}
				// ... add other primitives as needed
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
	}

	void GraphicsEngine::renderSceneHierarchy()
	{
		ImGui::Begin("Scene Hierarchy");

		for (size_t i = 0; i < m_gameObjects.size(); ++i)
		{
			GameObject* go = m_gameObjects[i].get();
			if (!go) continue;

			ImGui::PushID(i);

			const bool isSelected = (m_selectedObject == go);
			const std::string& name = go->getName();

			const char* displayName = name.empty() ? "(Unnamed Object)" : name.c_str();

			if (ImGui::Selectable(displayName, isSelected))
			{
				selectObject(i);
			}

			ImGui::PopID();
		}

		ImGui::End();
	}

	void GraphicsEngine::renderInspector()
	{
		ImGui::Begin("Inspector");

		if (m_selectedObject)
		{
			// --- NAME ---
			char nameBuffer[128];
			strncpy_s(nameBuffer, sizeof(nameBuffer), m_selectedObject->getName().c_str(), sizeof(nameBuffer) - 1);

			if (ImGui::InputText("##Name", nameBuffer, sizeof(nameBuffer)))
			{
				m_selectedObject->setName(nameBuffer);
			}

			ImGui::Separator();
			ImGui::Spacing();

			// --- TRANSFORM ---
			if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
			{
				Vec3 position = m_selectedObject->getPosition();
				if (ImGui::DragFloat3("Position", &position.x, 0.1f))
				{
					m_selectedObject->setPosition(position);
				}

				const float PI = 3.1415926535f;
				const float radToDeg = 180.0f / PI;
				const float degToRad = PI / 180.0f;

				Vec3 rotationInDegrees = m_selectedObject->getRotation() * radToDeg;
				if (ImGui::DragFloat3("Rotation", &rotationInDegrees.x, 1.0f))
				{
					m_selectedObject->setRotation(rotationInDegrees * degToRad);
				}

				Vec3 scale = m_selectedObject->getScale();
				if (ImGui::DragFloat3("Scale", &scale.x, 0.1f))
				{
					m_selectedObject->setScale(scale);
				}
			}

			ImGui::Spacing();

			// --- MATERIAL ---
			if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Checkbox("Use Custom Color", &m_selectedObject->m_overrideColor);

				if (!m_selectedObject->m_overrideColor)
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				}

				// If the user edits the color, we automatically re-enable the override for them.
				if (ImGui::ColorEdit4("Color", &m_selectedObject->m_color.x))
				{
					m_selectedObject->m_overrideColor = true;
				}

				// If the color picker was disabled, re-enable input for subsequent UI elements.
				if (!m_selectedObject->m_overrideColor)
				{
					ImGui::PopItemFlag();
					ImGui::PopStyleVar();
				}
			}

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			if (ImGui::Button("Delete Object", ImVec2(-1, 0)))
			{
				removeGameObject(m_selectedObject);
			}
		}
		else
		{
			ImGui::Text("No object selected.");
		}

		ImGui::End();
	}

	void GraphicsEngine::renderSceneView()
	{
		ImGui::Begin("Scene");

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		if (viewportPanelSize.x != m_sceneViewSize.x || viewportPanelSize.y != m_sceneViewSize.y)
		{
			recreateSceneResources(viewportPanelSize.x, viewportPanelSize.y);
			m_camera->setProjection(90.0f, viewportPanelSize.x / viewportPanelSize.y, 0.1f, 1000.0f);
		}

		ImGui::Image((void*)m_sceneShaderResourceView.Get(), viewportPanelSize);

		ImGui::End();
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