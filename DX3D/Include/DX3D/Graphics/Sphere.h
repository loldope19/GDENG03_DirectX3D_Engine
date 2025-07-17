#pragma once
#include <DX3D/Game/GameObject.h>
#include <DX3D/Graphics/Shader.h>

namespace dx3d
{
	class Sphere final : public GameObject
	{
	public:
		Sphere(const GraphicsResourceDesc& gDesc);

		void onUpdate(float dt) override;
		void render(GraphicsEngine* engine) override;

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
		std::unique_ptr<Shader> m_vertexShader;
		std::unique_ptr<Shader> m_pixelShader;

		UINT m_indexCount = 0;
		UINT m_stride = 0;
		UINT m_offset = 0;
	};
}