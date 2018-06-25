#pragma once

#define DX_VERSION_11

//#include "Engine/RHI/RHITypes.hpp"
#if defined(DEBUG)
	#define IGINID
#endif
#include <d3d11.h>
//#include <d3d10.h>
#include <DXGI.h>
#include <dxgi1_2.h>

// DEBUG STUFF
#include <dxgidebug.h>
// #pragma comment( lib, "dxguid.lib" )

#pragma comment( lib, "d3d11.lib" )
// #pragma comment( lib, "d3d10.lib" )
#pragma comment( lib, "DXGI.lib" )

// For Shader Compilation
#include <d3dcompiler.h>
#pragma comment( lib, "d3dcompiler.lib" )

#define DX_SAFE_RELEASE(dx_resource)   if ((dx_resource) != nullptr) { dx_resource->Release(); dx_resource = nullptr; }