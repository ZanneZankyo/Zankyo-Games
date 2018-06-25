
#include "Engine/RHI/Texture2D.hpp"
#include "Engine/RHI/RHIOutput.hpp"
#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/RHI/RHIDeviceContext.hpp"
#include "DX11.hpp"
#include "Engine/Core/EngineBase.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <wincodec.h>

Texture2D::Texture2D()
	:Texture2D(nullptr)
{
}

Texture2D::Texture2D(RHIDevice * device)
	: m_device(device)
	, m_dxResource(nullptr)
	, m_dxRtv(nullptr)
	, m_dxSrv(nullptr)
	, m_dxDsv(nullptr)
	, m_dxUav(nullptr)
	, m_width(0)
	, m_height(0)
	, m_dxBindFlags(0U)
{
}

Texture2D::Texture2D(RHIDevice * device, RHIOutput * output)
	: Texture2D(device)
{
	if (output != nullptr) {
		ID3D11Texture2D *back_buffer = nullptr;
		output->m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer);
		if (nullptr != back_buffer) {
			m_dxResource = back_buffer;

			// Get info about it.
			D3D11_TEXTURE2D_DESC desc;
			m_dxResource->GetDesc(&desc);

			// Set flags for backbuffer texture
			m_width = desc.Width;
			m_height = desc.Height;
			m_dxBindFlags = desc.BindFlags;

			DX_SAFE_RELEASE(back_buffer);

			CreateViews();
		}
	}
}

Texture2D::Texture2D(RHIDevice * device, const std::string & fileName, bool isUAV)
	:Texture2D(device)
{
	CreateTexture(Image(fileName), isUAV);
}

Texture2D::Texture2D(RHIDevice *device, const Image& image, bool isUAV)
	: Texture2D(device)
{
	CreateTexture(image, isUAV);
}

Texture2D::Texture2D(RHIDevice *owner, unsigned int w, unsigned int h, ImageFormat format, bool isUAV)
	: Texture2D(owner)
{
	// default usage - this is going to be written to by the GPU
	D3D11_USAGE usage = D3D11_USAGE_DEFAULT;
	m_width = w;
	m_height = h;

	DXGI_FORMAT dx_format;
	unsigned int dx_bindings = 0U;
	switch (format) {
	case IMAGEFORMAT_RGBA8:
		dx_format = DXGI_FORMAT_R8G8B8A8_UNORM;
		dx_bindings = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		break;
	case IMAGEFORMAT_D24S8:
		// depth textures are 24-bit depth, 8-bit stencil
		dx_format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dx_bindings = D3D11_BIND_DEPTH_STENCIL; // binding this to as a shader resource requires a couple extra steps - saved for later
		break;
	default:
		// ASSERTORDIE
		return;
	};

	if (isUAV)
		dx_bindings |= D3D11_BIND_UNORDERED_ACCESS;

	D3D11_TEXTURE2D_DESC tex_desc;
	memset(&tex_desc, 0, sizeof(tex_desc));

	tex_desc.Width = m_width;
	tex_desc.Height = m_height;
	tex_desc.MipLevels = 1;                // no mip mapping
	tex_desc.ArraySize = 1;                // NO ARRAYS!  
	tex_desc.Usage = usage;
	tex_desc.Format = dx_format;
	tex_desc.BindFlags = dx_bindings;
	tex_desc.CPUAccessFlags = 0U;
	tex_desc.MiscFlags = 0U; // there is one for generating mip maps.

							 // multisampling options
	tex_desc.SampleDesc.Count = 1;
	tex_desc.SampleDesc.Quality = 0;

	// no initial data - we're creating renderable targets

	ID3D11Device *dxDevice = m_device->m_dxDevice;
	HRESULT hr = dxDevice->CreateTexture2D(&tex_desc, nullptr, &m_dxResource);

	if (SUCCEEDED(hr)) {
		m_dxBindFlags = dx_bindings;
		CreateViews();
	}
}

Texture2D::Texture2D(RHIDevice *owner, ID3D11Texture2D* dxTexture)
	: Texture2D(owner)
{
	m_dxResource = dxTexture;
	// Get info about it.
	D3D11_TEXTURE2D_DESC desc;
	m_dxResource->GetDesc(&desc);

	// Set flags for backbuffer texture
	m_width = desc.Width;
	m_height = desc.Height;
	m_dxBindFlags = desc.BindFlags;
	m_dxBindFlags |= D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	CreateViews();
}

Texture2D::~Texture2D()
{
	DX_SAFE_RELEASE(m_dxResource);
	DX_SAFE_RELEASE(m_dxRtv);
	DX_SAFE_RELEASE(m_dxSrv);
	DX_SAFE_RELEASE(m_dxDsv);
}

bool Texture2D::CreateTexture(const Image& image, bool isUAV)
{

	D3D11_USAGE usage = D3D11_USAGE_DEFAULT;

	D3D11_TEXTURE2D_DESC texDesc;
	memset(&texDesc, 0, sizeof(texDesc));

	texDesc.Width = m_width = image.m_dimensions.x;
	texDesc.Height = m_height = image.m_dimensions.y;
	texDesc.MipLevels = 1;                // no mip mapping
	texDesc.ArraySize = 1;                // NO ARRAYS!  
	texDesc.Usage = usage;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	if (isUAV)
		texDesc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
	texDesc.CPUAccessFlags = 0U;
	texDesc.MiscFlags = 0U; // there is one for generating mip maps.

	
							 // multisampling options
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;

	// Helps us figure out what views to make
	m_dxBindFlags = texDesc.BindFlags;

	// Setup initial data
	D3D11_SUBRESOURCE_DATA data;
	MemSetZero(&data);
	//memset(&data, 0, sizeof(data));
	data.pSysMem = image.m_texelBytes;
	data.SysMemPitch = image.m_bytesPerTexel * texDesc.Width;

	ID3D11Device *dx_device = m_device->m_dxDevice;
	HRESULT hr = dx_device->CreateTexture2D(&texDesc, &data, &m_dxResource);

	if (SUCCEEDED(hr)) {
		CreateViews();
		return true;
	}
	else {
		return false;
	}
}

void Texture2D::CreateViews()
{

	ID3D11Device *dxDevice = m_device->m_dxDevice;

	if (m_dxBindFlags & D3D11_BIND_RENDER_TARGET) {
		dxDevice->CreateRenderTargetView(m_dxResource, nullptr, &m_dxRtv);
	}

	if (m_dxBindFlags & D3D11_BIND_SHADER_RESOURCE) {
		dxDevice->CreateShaderResourceView(m_dxResource, nullptr, &m_dxSrv);
	}

	if (m_dxBindFlags & D3D11_BIND_DEPTH_STENCIL) {
		dxDevice->CreateDepthStencilView(m_dxResource, nullptr, &m_dxDsv);
	}

	if (m_dxBindFlags & D3D11_BIND_UNORDERED_ACCESS) {
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
		MemSetZero(&desc);

		desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;;
		desc.Texture2D.MipSlice = 0;

		// NOTE:  Depth Stencil Buffers need to have a different view
		// then their normal 
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//ToDxFormat(m_format);

		HRESULT hr = dxDevice->CreateUnorderedAccessView(m_dxResource, &desc, &m_dxUav);
		ASSERT_RECOVERABLE(SUCCEEDED(hr), "Texture2D::CreateViews(): failed to create uav.\n");
	}
}

DXGI_FORMAT Texture2D::ToDxFormat(ImageFormat format)
{
	switch (format)
	{
	case IMAGEFORMAT_RGBA8:
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	case IMAGEFORMAT_D24S8:
		return DXGI_FORMAT_D24_UNORM_S8_UINT;
	default:
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	}
}

/*

Reference: https://github.com/Microsoft/graphics-driver-samples/blob/master/render-only-sample/rostest/util.cpp#L244

*/

Image* Texture2D::ToImage()
{
	HRESULT hr;

	// First verify that we can map the texture
	D3D11_TEXTURE2D_DESC desc;
	m_dxResource->GetDesc(&desc);

	// translate texture format to WIC format. We support only BGRA and ARGB.
	GUID wicFormatGuid;
	switch (desc.Format) {
	case DXGI_FORMAT_R8G8B8A8_UNORM:
		wicFormatGuid = GUID_WICPixelFormat32bppRGBA;
		break;
	case DXGI_FORMAT_B8G8R8A8_UNORM:
		wicFormatGuid = GUID_WICPixelFormat32bppBGRA;
		break;
	default:
		return nullptr;
		/*throw MyException::Make(
			HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED),
			L"Unsupported DXGI_FORMAT: %d. Only RGBA and BGRA are supported.",
			desc.Format);*/
	}

	// Get the device context
	ID3D11Device* d3dDevice = m_device->m_dxDevice;
	m_dxResource->GetDevice(&d3dDevice);
	ID3D11DeviceContext* d3dContext = m_device->m_immediateContext->m_dxContext;
	d3dDevice->GetImmediateContext(&d3dContext);

	// map the texture
	ID3D11Texture2D* mappedTexture;
	D3D11_MAPPED_SUBRESOURCE mapInfo;
	//mapInfo.RowPitch;
	hr = d3dContext->Map(
		m_dxResource,
		0,  // Subresource
		D3D11_MAP_READ,
		0,  // MapFlags
		&mapInfo);

	if (FAILED(hr)) {
		// If we failed to map the texture, copy it to a staging resource
		if (hr == E_INVALIDARG) {
			D3D11_TEXTURE2D_DESC desc2;
			desc2.Width = desc.Width;
			desc2.Height = desc.Height;
			desc2.MipLevels = desc.MipLevels;
			desc2.ArraySize = desc.ArraySize;
			desc2.Format = desc.Format;
			desc2.SampleDesc = desc.SampleDesc;
			desc2.Usage = D3D11_USAGE_STAGING;
			desc2.BindFlags = 0;
			desc2.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			desc2.MiscFlags = 0;

			ID3D11Texture2D* stagingTexture;
			hr = d3dDevice->CreateTexture2D(&desc2, nullptr, &stagingTexture);
			if (FAILED(hr)) {
				return nullptr;
				//throw MyException::Make(hr, L"Failed to create staging texture");
			}

			// copy the texture to a staging resource
			d3dContext->CopyResource(stagingTexture, m_dxResource);

			// now, map the staging resource
			hr = d3dContext->Map(
				stagingTexture,
				0,
				D3D11_MAP_READ,
				0,
				&mapInfo);
			if (FAILED(hr)) {
				//throw MyException::Make(hr, L"Failed to map staging texture");
				return nullptr;
			}

			mappedTexture = std::move(stagingTexture);
		}
		else {
			return nullptr;
			//throw MyException::Make(hr, L"Failed to map texture.");
		}
	}
	else {
		mappedTexture = m_dxResource;
	}
	byte_t* bytes = (byte_t*)mapInfo.pData;
	
	Image* img = new Image(bytes, 4, IntVector2(mapInfo.RowPitch / 4, mapInfo.DepthPitch / mapInfo.RowPitch));
	img->BgraSelf();

	/*auto unmapResource = Finally([&] {
		d3dContext->Unmap(mappedTexture.Get(), 0);
	});*/
	d3dContext->Unmap(mappedTexture, 0);

	DX_SAFE_RELEASE(mappedTexture);

	//free(mapInfo.pData);
	return img;

	/*IWICImagingFactory* wicFactory;
	hr = CoCreateInstance(
		CLSID_WICImagingFactory,
		nullptr,
		CLSCTX_INPROC_SERVER,
		__uuidof(wicFactory),
		reinterpret_cast<void**>(&wicFactory));
	if (FAILED(hr)) {
		/ *throw MyException::Make(
			hr,
			L"Failed to create instance of WICImagingFactory");* /
		return nullptr;
	}

	IWICBitmapEncoder* wicEncoder;
	hr = wicFactory->CreateEncoder(
		GUID_ContainerFormatBmp,
		nullptr,
		&wicEncoder);
	if (FAILED(hr)) {
		//throw MyException::Make(hr, L"Failed to create BMP encoder");
		return nullptr;
	}

	IWICStream* wicStream;
	hr = wicFactory->CreateStream(&wicStream);
	if (FAILED(hr)) {
		//throw MyException::Make(hr, L"Failed to create IWICStream");
		return nullptr;
	}

	/ *hr = wicStream->InitializeFromFilename(FileName, GENERIC_WRITE);
	if (FAILED(hr)) {
		throw MyException::Make(hr, L"Failed to initialize stream from file name");
	}

	hr = wicEncoder->Initialize(wicStream.Get(), WICBitmapEncoderNoCache);
	if (FAILED(hr)) {
		throw MyException::Make(hr, L"Failed to initialize bitmap encoder");
	}* /

	// Encode and commit the frame
	{
		IWICBitmapFrameEncode* frameEncode;
		wicEncoder->CreateNewFrame(&frameEncode, nullptr);
		if (FAILED(hr)) {
			throw MyException::Make(hr, L"Failed to create IWICBitmapFrameEncode");
		}

		hr = frameEncode->Initialize(nullptr);
		if (FAILED(hr)) {
			throw MyException::Make(hr, L"Failed to initialize IWICBitmapFrameEncode");
		}


		hr = frameEncode->SetPixelFormat(&wicFormatGuid);
		if (FAILED(hr)) {
			throw MyException::Make(
				hr,
				L"SetPixelFormat(%s) failed.",
				StringFromWicFormat(wicFormatGuid));
		}

		hr = frameEncode->SetSize(desc.Width, desc.Height);
		if (FAILED(hr)) {
			throw MyException::Make(hr, L"SetSize(...) failed.");
		}

		hr = frameEncode->WritePixels(
			desc.Height,
			mapInfo.RowPitch,
			desc.Height * mapInfo.RowPitch,
			reinterpret_cast<BYTE*>(mapInfo.pData));
		if (FAILED(hr)) {
			throw MyException::Make(hr, L"frameEncode->WritePixels(...) failed.");
		}

		hr = frameEncode->Commit();
		if (FAILED(hr)) {
			throw MyException::Make(hr, L"Failed to commit frameEncode");
		}
	}

	hr = wicEncoder->Commit();
	if (FAILED(hr)) {
		throw MyException::Make(hr, L"Failed to commit encoder");
	}*/
}