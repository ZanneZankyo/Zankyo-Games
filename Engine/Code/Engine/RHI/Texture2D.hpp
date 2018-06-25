#pragma once

#include <string>
#include "Engine/RHI/DX11.hpp"
#include "Engine/Renderer/Image.hpp"
#include "RHITypes.hpp"

enum ImageFormat : unsigned int
{
	IMAGEFORMAT_RGBA8,
	IMAGEFORMAT_D24S8,
};

class Texture2D
{
public:
	Texture2D();
	Texture2D(RHIDevice *device);
	Texture2D(RHIDevice *device, RHIOutput *output);
	Texture2D(RHIDevice *device, const std::string& fileName, bool isUAV = false);
	Texture2D(RHIDevice *device, const Image& image, bool isUAV = false);
	Texture2D(RHIDevice *device, unsigned int width, unsigned int height, ImageFormat format, bool isUAV = false);
	Texture2D(RHIDevice *device, ID3D11Texture2D* dxTexture);
	~Texture2D();

	UINT GetWidth();
	UINT GetHeight();

	inline bool IsValid() const { return (m_dxResource != nullptr); }
	inline bool IsRenderTarget() const { return (m_dxRtv != nullptr); }

	bool CreateTexture(const Image& image, bool isUAV = false);
	void CreateViews();

	static DXGI_FORMAT ToDxFormat(ImageFormat format);

	Image* ToImage();
public:
	RHIDevice *m_device;

	ID3D11Texture2D *m_dxResource;
	ID3D11RenderTargetView *m_dxRtv;
	ID3D11ShaderResourceView *m_dxSrv;
	ID3D11DepthStencilView *m_dxDsv;
	ID3D11UnorderedAccessView *m_dxUav;

	UINT m_width;
	UINT m_height;
	UINT m_dxBindFlags;
};


