#include <DX3D/Graphics/DeviceContext.h>
#include <DX3D/Graphics/SwapChain.h>
#include <DX3D/Graphics/GraphicsPipelineState.h>

dx3d::DeviceContext::DeviceContext(const GraphicsResourceDesc& gDesc) : GraphicsResource(gDesc)
{
	DX3DGraphicsLogThrowOnFail(m_device.CreateDeferredContext(0, &m_context),
		"CreateDeferredContext failed.");
}

dx3d::DeviceContext::DeviceContext(const GraphicsResourceDesc& gDesc, const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context)
	: GraphicsResource(gDesc), m_context(context)
{
}

void dx3d::DeviceContext::clearAndSetBackBuffer(const SwapChain& swapChain, const Vec4& color)
{
	f32 fColor[] = { color.x,color.y,color.z,color.w };
	auto rtv = swapChain.m_rtv.Get();
	auto dsv = swapChain.m_dsv.Get();

	m_context->ClearRenderTargetView(rtv, fColor);
	m_context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH, 1.0f, 0);

	m_context->OMSetRenderTargets(1, &rtv, dsv);
}

void dx3d::DeviceContext::setGraphicsPipelineState(const GraphicsPipelineState& pipeline)
{
	m_context->VSSetShader(pipeline.m_vs.Get(), nullptr, 0);
	m_context->PSSetShader(pipeline.m_ps.Get(), nullptr, 0);
}