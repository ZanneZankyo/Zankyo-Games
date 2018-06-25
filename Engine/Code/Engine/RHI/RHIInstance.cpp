#include "RHIInstance.hpp"
#include "RHIOutput.hpp"
#include "Engine\Core\EngineConfig.hpp"

#define NO_RENDER_DEBUGING

RHIInstance::RHIInstance() 
	:m_debug(nullptr)
	,m_debugModule(nullptr)
{
	
#if defined (RENDER_DEBUGING)
	m_debugModule = LoadLibraryEx("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	typedef HRESULT(WINAPI *GetDebugModuleCB)(REFIID, void**);
	GetDebugModuleCB cb = (GetDebugModuleCB) ::GetProcAddress(m_debugModule, "DXGIGetDebugInterface");	        
	cb(__uuidof(IDXGIDebug), (void**)&m_debug);	       
	m_debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
#endif

}

RHIInstance RHIInstance::s_instance;

bool RHIInstance::CreateOutput(RHIDevice *&outDevice,
	RHIDeviceContext *&outContext,
	RHIOutput *&outOutput,
	unsigned int const pxWidth,
	unsigned int const pxHeight)
{
	Window *window = new Window(pxWidth, pxHeight);

	// Creation Flags
	// For options, see;
	// https://www.google.com/webhp?sourceid=chrome-instant&ion=1&espv=2&ie=UTF-8#safe=off&q=device_flags+%7C%3D+D3D11_CREATE_DEVICE_DEBUG%3B
	UINT device_flags = 0U;
#if defined(RENDER_DEBUG)
	device_flags |= D3D11_CREATE_DEVICE_DEBUG;

	// This flag fails unless we' do 11.1 (which we're not), and we query that
	// the adapter support its (which we're not).  Just here to let you know it exists.
	// device_flags |= D3D11_CREATE_DEVICE_DEBUGGABLE; 
#endif

	// Setup our Swap Chain
	// For options, see;
	// https://www.google.com/webhp?sourceid=chrome-instant&ion=1&espv=2&ie=UTF-8#safe=off&q=DXGI_SWAP_CHAIN_DESC

	DXGI_SWAP_CHAIN_DESC swap_desc;
	memset(&swap_desc, 0, sizeof(swap_desc));

	// fill the swap chain description struct
	swap_desc.BufferCount = 2;                                    // two buffers (one front, one back?)

	swap_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT       // how swap chain is to be used
		| DXGI_USAGE_BACK_BUFFER;
	swap_desc.OutputWindow = window->m_windowHandle;    // the window to be copied to on present
	swap_desc.SampleDesc.Count = 1;                               // how many multisamples (1 means no multi sampling)

																  // Default options.
	swap_desc.Windowed = TRUE;                                    // windowed/full-screen mode
	swap_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color



	swap_desc.BufferDesc.Width = pxWidth;//g_windowWidth;
	swap_desc.BufferDesc.Height = pxHeight;//g_windowHeight;

	ID3D11Device *dxDevice;
	ID3D11DeviceContext *dxContext;
	IDXGISwapChain *dxSwapchain;

	// Actually Create
	HRESULT hr = ::D3D11CreateDeviceAndSwapChain(nullptr, // Adapter, if nullptr, will use adapter window is primarily on.
		D3D_DRIVER_TYPE_HARDWARE,		// Driver Type - We want to use the GPU (HARDWARE)
		nullptr,						// Software Module - DLL that implements software mode (we do not use)
		device_flags,					// device creation options
		nullptr,						// feature level (use default)
		0U,								// number of feature levels to attempt
		D3D11_SDK_VERSION,				// SDK Version to use
		&swap_desc,						// Description of our swap chain
		&dxSwapchain,		// Swap Chain we're creating
		&dxDevice,	// [out] The device created
		nullptr,						// [out] Feature Level Acquired
		&dxContext
		/*&(outContext.m_dxContext)*/);	// Context that can issue commands on this pipe.

	//outContext.m_dxContext = outOutput.m_device->immediate_context->m_dxContext;

	if (SUCCEEDED(hr))
	{
		RHIDevice *device = new RHIDevice(this, dxDevice);
		RHIDeviceContext *context = new RHIDeviceContext(device, dxContext);
		RHIOutput *output = new RHIOutput(device, dxSwapchain, window);

		device->m_immediateContext = context;

		outDevice = device;
		outContext = context;
		outOutput = output;

		return true;
	}

								  // SUCCEEDED & FAILED are macros provided by Windows to checking
								  // the results.  Almost every D3D call will return one - be sure to check it.
	return SUCCEEDED(hr);
}