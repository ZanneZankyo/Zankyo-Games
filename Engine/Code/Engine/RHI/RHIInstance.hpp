#pragma once
#include "DX11.hpp"
#include "RHIDevice.hpp"
#include "RHIDeviceContext.hpp"

class RHIInstance
{
public:
	// Constructor is privete:  Singleton Class
	~RHIInstance();

	bool RHIInstance::CreateOutput(RHIDevice *&outDevice,
		RHIDeviceContext *&outContext,
		RHIOutput *&outOutput,
		unsigned int const pxWidth,
		unsigned int const pxHeight /*,
		OPTIONAL eRHIOutputMode const initial_mode = RENDEROUTPUT_WINDOWED*/);

private:
	RHIInstance();

	static RHIInstance s_instance;

	IDXGIDebug* m_debug;
	HMODULE m_debugModule;
public:
	static RHIInstance* GetInstance();
};



inline RHIInstance::~RHIInstance() {}

inline RHIInstance* RHIInstance::GetInstance()
{
	return &s_instance;
}