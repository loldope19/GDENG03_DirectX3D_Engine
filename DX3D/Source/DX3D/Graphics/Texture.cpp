#include <DX3D/Graphics/Texture.h>
#include <DX3D/Graphics/GraphicsLogUtils.h>
#include <wincodec.h>
#include <shlwapi.h>
#include <vector>
#include <iostream>

#pragma comment(lib, "windowscodecs.lib")

namespace dx3d
{
    Texture::Texture(const GraphicsResourceDesc& desc, const std::wstring& filePath) : GraphicsResource(desc)
    {
        if (!PathFileExistsW(filePath.c_str()))
        {
            std::string narrowFilePath(filePath.begin(), filePath.end());
            std::cout << "Texture file not found: " + narrowFilePath << std::endl;
        }

        // Initialize COM for WIC usage
        CoInitialize(NULL);

        Microsoft::WRL::ComPtr<IWICImagingFactory> wicFactory;
        DX3DGraphicsLogThrowOnFail(
            CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory)),
            "Failed to create WIC Imaging Factory"
        );

        Microsoft::WRL::ComPtr<IWICBitmapDecoder> wicDecoder;
        DX3DGraphicsLogThrowOnFail(
            wicFactory->CreateDecoderFromFilename(filePath.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &wicDecoder),
            "Failed to create WIC decoder from file"
        );

        Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> wicFrame;
        DX3DGraphicsLogThrowOnFail(wicDecoder->GetFrame(0, &wicFrame), "Failed to get WIC frame");

        Microsoft::WRL::ComPtr<IWICFormatConverter> wicConverter;
        DX3DGraphicsLogThrowOnFail(wicFactory->CreateFormatConverter(&wicConverter), "Failed to create WIC format converter");

        DX3DGraphicsLogThrowOnFail(
            wicConverter->Initialize(wicFrame.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut),
            "Failed to initialize WIC format converter"
        );

        UINT width, height;
        DX3DGraphicsLogThrowOnFail(wicConverter->GetSize(&width, &height), "Failed to get image size from WIC converter");

        std::vector<BYTE> buffer(width * height * 4);
        DX3DGraphicsLogThrowOnFail(wicConverter->CopyPixels(NULL, width * 4, (UINT) buffer.size(), buffer.data()), "Failed to copy pixels using WIC");

        D3D11_TEXTURE2D_DESC texDesc = {};
        texDesc.Width = width;
        texDesc.Height = height;
        texDesc.MipLevels = 1;
        texDesc.ArraySize = 1;
        texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        texDesc.SampleDesc.Count = 1;
        texDesc.Usage = D3D11_USAGE_DEFAULT;
        texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = buffer.data();
        initData.SysMemPitch = width * 4;

        Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2D;
        DX3DGraphicsLogThrowOnFail(m_device.CreateTexture2D(&texDesc, &initData, &texture2D), "Failed to create D3D11 texture from WIC data");

        DX3DGraphicsLogThrowOnFail(m_device.CreateShaderResourceView(texture2D.Get(), NULL, &m_shaderResourceView), "Failed to create shader resource view from texture");

        CoUninitialize();
    }
}