#pragma once
#include <DX3D/Game/GameObject.h>
#include <DX3D/Graphics/Shader.h>
#include <DX3D/Game/PropertyAnimator.h>

namespace dx3d
{
	class Shader;

	class Cube : public GameObject
	{
	public:
		Cube(const GraphicsResourceDesc& gDesc);

		void update(float dt) override;
		void render(GraphicsEngine* engine) override;

	protected:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;

		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
		std::unique_ptr<Shader> m_vertexShader;
		std::unique_ptr<Shader> m_pixelShader;

		UINT m_stride = 0;
		UINT m_offset = 0;

		PropertyAnimator m_scaleAnimator;
		PropertyAnimator m_positionAnimator;
		Vec3 m_rotationSpeed;
	};
}