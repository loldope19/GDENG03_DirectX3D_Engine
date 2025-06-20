#pragma once
#include <DX3D/Core/Base.h>
#include <DX3D/Core/Common.h>
#include <DX3D/Game/Camera.h>
#include <DX3D/Graphics/GraphicsResource.h>
#include <memory>
#include <vector>
#include <d3d11.h>
#include <wrl.h>

namespace dx3d
{
	class SwapChain;
	class DeviceContext;
	class GraphicsDevice;
	class GameObject;
	class GraphicsPipelineState;

	class GraphicsEngine final : public Base
	{
	public:
		explicit GraphicsEngine(const GraphicsEngineDesc& desc);
		virtual ~GraphicsEngine() noexcept;

		GraphicsDevice& getGraphicsDevice() const;
		GraphicsResourceDesc getGraphicsResourceDesc() const;

		void render(SwapChain& swapChain);
		void onUpdate(float dt);
		void addGameObject(std::unique_ptr<GameObject> go);
		void selectObject(size_t index);

		DeviceContextPtr getDeviceContext() const { return m_deviceContext; }
		Matrix4x4 getViewMatrix() const;
		Matrix4x4 getProjectionMatrix() const;

		void updateConstantBuffer(const Matrix4x4& world, const Matrix4x4& view, const Matrix4x4& projection);

	private:
		friend class GameObject;

		GameObject* m_selectedObject = nullptr;

		std::shared_ptr<GraphicsDevice> m_graphicsDevice;
		DeviceContextPtr m_deviceContext;

		std::unique_ptr<Camera> m_camera;
		std::vector<std::unique_ptr<GameObject>> m_gameObjects;

		Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_materialConstantBuffer;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerStateCullNone;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerStateCullBack;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;
		GraphicsPipelineStatePtr m_pipeline;

	};
}