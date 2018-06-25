#pragma once

//#include "Engine/RHI/RHIDevice.hpp"
//#include "Texture2D.hpp"
#include "Engine/Core/Window.hpp"
#include "DX11.hpp"

class RHIDevice;
class Texture2D;
class D3D11Renderer;

class RHIOutput
{
public:
	RHIOutput(RHIDevice *owner,
		IDXGISwapChain *swapChain,
		Window *wnd/*,
		[OPTIONAL] eRHIOutputMode const mode = RENDEROUTPUT_WINDOWED*/);
	~RHIOutput();

	void Present();
	void Close();

	Texture2D* GetRenderTarget();
	bool CreateRenderTarget();
	float GetWidth() const;
	float GetHeight() const;
	float GetAspectRatio() const;

	void ResizeBuffer(int width, int height);

	// [OPTIONAL]
	/*bool set_display_mode(eRHIOutputMode const mode,
		float const width = 0.0f,
		float const height = 0.0f);*/

public:
	RHIDevice *m_device;
	Window *m_window;

	IDXGISwapChain *m_swapChain;

	Texture2D *m_renderTarget;

	D3D11Renderer* m_shader;

};

inline float RHIOutput::GetWidth() const
{
	return (float)m_window->GetWindowSize().x;
}

inline float RHIOutput::GetHeight() const
{
	return (float)m_window->GetWindowSize().y;
}

inline float RHIOutput::GetAspectRatio() const
{
	return GetWidth() / GetHeight();
}