#pragma once

#include <DX3D/Graphics/GraphicsResource.h>
#include <DX3D/Graphics/Shader.h>
#include <memory>
#include <vector>

namespace dx3d
{
	struct CircleVertex {
		float x, y, z;
		float r, g, b, a;
	};

	class Circle : public GraphicsResource {
	public:
		Circle(const GraphicsResourceDesc& gDesc);

		bool initializeSharedResources();
		void createCircle(const std::vector<CircleVertex>& vertices,
						const std::vector<UINT>& indices);
		void render(ID3D11DeviceContext& context);
		size_t getCircleCount() const { return m_vertexBuffers.size(); }

	private:
		std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> m_vertexBuffers;
		std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> m_indexBuffers;
		std::vector<UINT> m_indexCounts;

		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
		std::unique_ptr<Shader> m_vertexShader;
		std::unique_ptr<Shader> m_pixelShader;
		UINT m_stride;
		UINT m_offset;

		bool m_sharedResourcesInitialized = false;
	};
};

