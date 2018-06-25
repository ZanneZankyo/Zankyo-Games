#include "RHIOutput.hpp"
#include "Texture2D.hpp"
#include "RHIDevice.hpp"
#include "RHIDeviceContext.hpp"
#include "DX11.hpp"
#include "Engine/Renderer/D3D11Renderer.hpp"

RHIOutput::RHIOutput(RHIDevice *owner,
	IDXGISwapChain *swapChain,
	Window *wnd)
	: m_device(owner)
	, m_window(wnd)
	, m_swapChain(swapChain)
	, m_renderTarget(nullptr)
	, m_shader(nullptr)
{
	m_renderTarget = new Texture2D(owner, this);
	wnd->m_rhiOutput = this;
	CreateRenderTarget();
}

RHIOutput::~RHIOutput()
{
}

Texture2D* RHIOutput::GetRenderTarget()
{
	return m_renderTarget;
}

bool RHIOutput::CreateRenderTarget()
{
	/*ID3D11Texture2D *back_buffer = nullptr;
	m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer);
	m_renderTarget = new Texture2D(m_device, back_buffer);
	DX_SAFE_RELEASE(back_buffer);


	return true;*/
	m_renderTarget = new Texture2D(m_device, this);
	return m_renderTarget->IsValid();
}

void RHIOutput::Present()
{
						  // We're done rendering, so tell the swap chain they can copy the back buffer to the front (desktop/window) buffer
	m_swapChain->Present(1, // Sync Interval, set to 1 for VSync
		0);                    // Present flags, see;
							   // https://msdn.microsoft.com/en-us/library/windows/desktop/bb509554(v=vs.85).aspx
}

void RHIOutput::ResizeBuffer(int width, int height)
{

	m_device->m_immediateContext->SetRenderTarget(nullptr);

	SAFE_DELETE(m_renderTarget);
	
	HRESULT hr;
	// Preserve the existing buffer count and format.
	// Automatically choose the width and height to match the client rect for HWNDs.
	//hr = m_swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
	hr = m_swapChain->ResizeBuffers(2, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, NULL);

	bool resizeResult = SUCCEEDED(hr);
	UNUSED(resizeResult);

	// Get buffer and create a render-target-view.
	m_renderTarget = new Texture2D(m_device, this);
	m_shader->SetRenderTarget(m_renderTarget);
	//m_device->m_immediateContext->SetRenderTarget(m_renderTarget);
	
	// Set up the viewport.
	/*D3D11_VIEWPORT vp;
	vp.Width = width;
	vp.Height = height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_pd3dDeviceContext->RSSetViewports(1, &vp);*/


	IntVector2 viewPorts = m_window->GetWindowSize();

	m_device->m_immediateContext->SetViewports(0, 0, viewPorts.x, viewPorts.y);
	
}