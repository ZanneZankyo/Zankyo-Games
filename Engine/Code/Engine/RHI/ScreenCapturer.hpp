#pragma once

#include "Engine/RHI/DX11.hpp"
#include "RHITypes.hpp"
#include "Engine/Core/EngineBase.hpp"
#include "Engine/Renderer/Image.hpp"
/*
#include <NvAPI/nvapi.h>
#include <NvFBC/nvFBC.h>
#include <NvFBC/nvFBCToSys.h>
#include "ThirdParty/NvFBC/NvFBCLibrary.h"*/

/*
struct AppArguments
{
	int   iFrameCnt; // Number of frames to grab
	int   iStartX; // Grab start x coord
	int   iStartY; // Grab start y coord
	int   iWidth; // Grab width
	int   iHeight; // Grab height
	int   iSetUpFlags; // Set up flags
	bool  bHWCursor; // Grab hardware cursor
	NVFBCToSysGrabMode     eGrabMode; // Grab mode
	NVFBCToSysBufferFormat eBufFormat; // Output buffer format
	std::string sBaseName; // Output file name
	FILE*       yuvFile;
	AppArguments()
		: iFrameCnt(0)
		, iStartX(0)
		, iStartY(0)
		, iWidth(0)
		, iHeight(0)
		, iSetUpFlags(0)
		, bHWCursor(false)
		, eGrabMode(NVFBC_TOSYS_SOURCEMODE_FULL)
		, eBufFormat(NVFBC_TOSYS_ARGB)
		, yuvFile(NULL)
	{
		sBaseName.clear();
	}
};*/

class ScreenCapturer
{
public:
	ScreenCapturer();
	~ScreenCapturer();
	bool Initialize(RHIDevice* device, uint output = 0);
	void EndFrame();
	Texture2D* GetFrame();
	Image* GetImage();

public:
	IDXGIOutputDuplication* m_DeskDupl;
	ID3D11Texture2D* m_AcquiredDesktopImage;
	byte_t* m_MetaDataBuffer;
	uint m_MetaDataSize;
	uint m_OutputNumber;
	DXGI_OUTPUT_DESC m_OutputDesc;
	RHIDevice* m_Device;

	/*NvFBCLibrary nvfbcLibrary;
	NvFBCToSys *nvfbcToSys = NULL;

	DWORD maxDisplayWidth = -1, maxDisplayHeight = -1;
	BOOL bRecoveryDone = FALSE;

	NvFBCFrameGrabInfo grabInfo;
	unsigned char *frameBuffer = NULL;
	unsigned char *diffMap = NULL;
	char frameNo[10];
	std::string outName;*/
};