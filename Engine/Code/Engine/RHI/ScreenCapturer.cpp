#include "ScreenCapturer.hpp"
#include "Texture2D.hpp"
#include "RHIDevice.hpp"
#include "..\Core\ErrorWarningAssert.hpp"
#include "..\Core\Window.hpp"


ScreenCapturer::ScreenCapturer() 
	: m_DeskDupl(nullptr)
	, m_AcquiredDesktopImage(nullptr)
	, m_MetaDataBuffer(nullptr)
	, m_MetaDataSize(0)
	, m_OutputNumber(0)
	, m_Device(nullptr)
{

}

ScreenCapturer::~ScreenCapturer()
{
	//DX_SAFE_RELEASE(m_DeskDupl);
	if (m_DeskDupl)
	{
		m_DeskDupl->Release();
		m_DeskDupl = nullptr;
	}

	if (m_AcquiredDesktopImage)
	{
		m_AcquiredDesktopImage->Release();
		m_AcquiredDesktopImage = nullptr;
	}

	if (m_MetaDataBuffer)
	{
		delete[] m_MetaDataBuffer;
		m_MetaDataBuffer = nullptr;
	}
}

bool ScreenCapturer::Initialize(RHIDevice* device, uint output /*= 0*/)
{
	/*ASSERT_OR_DIE(nvfbcLibrary.load(), "Failed to load NvFBC library");
	
	nvfbcToSys = (NvFBCToSys *)nvfbcLibrary.create(NVFBC_TO_SYS, &maxDisplayWidth, &maxDisplayHeight);

	NVFBCRESULT status = NVFBC_SUCCESS;

	ASSERT_OR_DIE(nvfbcToSys, "Unable to create an instance of NvFBC");

	//! Setup the frame grab
	NVFBC_TOSYS_SETUP_PARAMS fbcSysSetupParams = { 0 };
	fbcSysSetupParams.dwVersion = NVFBC_TOSYS_SETUP_PARAMS_VER;
	fbcSysSetupParams.eMode = NVFBC_TOSYS_RGB;
	fbcSysSetupParams.bWithHWCursor = args.bHWCursor;
	fbcSysSetupParams.bDiffMap = FALSE;
	fbcSysSetupParams.ppBuffer = (void **)&frameBuffer;
	fbcSysSetupParams.ppDiffMap = NULL;

	status = nvfbcToSys->NvFBCToSysSetUp(&fbcSysSetupParams);
	if (status == NVFBC_SUCCESS)
	{
		//! Sleep so that ToSysSetUp forces a framebuffer update
		Sleep(100);

		NVFBC_TOSYS_GRAB_FRAME_PARAMS fbcSysGrabParams = { 0 };
		//! For each frame to grab..
		for (int cnt = 0; cnt < args.iFrameCnt; ++cnt)
		{
			outName = args.sBaseName + "_" + _itoa(cnt, frameNo, 10) + ".bmp";
			//! Grab the frame.  
			// If scaling or cropping is enabled the width and height returned in the
			// NvFBCFrameGrabInfo structure reflect the current desktop resolution, not the actual grabbed size.
			fbcSysGrabParams.dwVersion = NVFBC_TOSYS_GRAB_FRAME_PARAMS_VER;
			fbcSysGrabParams.dwFlags = args.iSetUpFlags;
			fbcSysGrabParams.dwTargetWidth = args.iWidth;
			fbcSysGrabParams.dwTargetHeight = args.iHeight;
			fbcSysGrabParams.dwStartX = args.iStartX;
			fbcSysGrabParams.dwStartY = args.iStartY;
			fbcSysGrabParams.eGMode = args.eGrabMode;
			fbcSysGrabParams.pNvFBCFrameGrabInfo = &grabInfo;

			status = nvfbcToSys->NvFBCToSysGrabFrame(&fbcSysGrabParams);
			if (status == NVFBC_SUCCESS)
			{
				bRecoveryDone = FALSE;
				//! Save the frame to disk
				switch (args.eBufFormat)
				{
				case NVFBC_TOSYS_ARGB:
					SaveARGB(outName.c_str(), frameBuffer, grabInfo.dwWidth, grabInfo.dwHeight, grabInfo.dwBufferWidth);
					fprintf(stderr, "Grab succeeded. Wrote %s as ARGB.\n", outName.c_str());
					break;

				case NVFBC_TOSYS_RGB:
					SaveRGB(outName.c_str(), frameBuffer, grabInfo.dwWidth, grabInfo.dwHeight, grabInfo.dwBufferWidth);
					fprintf(stderr, "Grab succeeded. Wrote %s as RGB.\n", outName.c_str());
					break;

				case NVFBC_TOSYS_YUV444p:
					if (args.yuvFile) {
						fwrite(frameBuffer, grabInfo.dwWidth*grabInfo.dwHeight * 3, 1, args.yuvFile);
					}
					else {
						SaveYUV444(outName.c_str(), frameBuffer, grabInfo.dwWidth, grabInfo.dwHeight);
						fprintf(stderr, "Grab succeeded. Wrote %s as YUV444 converted to RGB.\n", outName.c_str());
					}
					break;

				case NVFBC_TOSYS_YYYYUV420p:
					if (args.yuvFile) {
						fwrite(frameBuffer, grabInfo.dwWidth*grabInfo.dwHeight * 3 / 2, 1, args.yuvFile);
					}
					else {
						SaveYUV420(outName.c_str(), frameBuffer, grabInfo.dwWidth, grabInfo.dwHeight);
						fprintf(stderr, "Grab succeeded. Wrote %s as YYYYUV420p.\n", outName.c_str());
					}
					break;

				case NVFBC_TOSYS_RGB_PLANAR:
					SaveRGBPlanar(outName.c_str(), frameBuffer, grabInfo.dwWidth, grabInfo.dwHeight);
					fprintf(stderr, "Grab succeeded. Wrote %s as RGB_PLANAR.\n", outName.c_str());
					break;

				case NVFBC_TOSYS_XOR:
					// The second grab results in the XOR of the first and second frame.
					fbcSysGrabParams.dwVersion = NVFBC_TOSYS_GRAB_FRAME_PARAMS_VER;
					fbcSysGrabParams.dwFlags = args.iSetUpFlags;
					fbcSysGrabParams.dwTargetWidth = args.iWidth;
					fbcSysGrabParams.dwTargetHeight = args.iHeight;
					fbcSysGrabParams.dwStartX = 0;
					fbcSysGrabParams.dwStartY = 0;
					fbcSysGrabParams.eGMode = args.eGrabMode;
					fbcSysGrabParams.pNvFBCFrameGrabInfo = &grabInfo;
					status = nvfbcToSys->NvFBCToSysGrabFrame(&fbcSysGrabParams);
					if (status == NVFBC_SUCCESS)
						SaveRGB(outName.c_str(), frameBuffer, grabInfo.dwWidth, grabInfo.dwHeight, grabInfo.dwBufferWidth);

					fprintf(stderr, "Grab succeeded. Wrote %s as XOR.\n", outName.c_str());
					break;
				case NVFBC_TOSYS_ARGB10:
					SaveARGB10(outName.c_str(), frameBuffer, grabInfo.dwWidth, grabInfo.dwHeight, grabInfo.dwBufferWidth);
					fprintf(stderr, "Grab succeeded. Wrote %s as ARGB10.\n", outName.c_str());
					break;

				default:
					fprintf(stderr, "Un-expected grab format %d.", args.eBufFormat);
					break;
				}
			}
			else
			{
				if (bRecoveryDone == TRUE)
				{
					fprintf(stderr, "Unable to recover from NvFBC Frame grab failure.\n");
					goto quit;
				}

				if (status == NVFBC_ERROR_DYNAMIC_DISABLE)
				{
					fprintf(stderr, "NvFBC disabled. Quitting\n");
					goto quit;
				}

				if (status == NVFBC_ERROR_INVALIDATED_SESSION)
				{
					fprintf(stderr, "Session Invalidated. Attempting recovery\n");
					nvfbcToSys->NvFBCToSysRelease();
					nvfbcToSys = NULL;
					//! Recover from error. Create an instance of NvFBCToSys
					nvfbcToSys = (NvFBCToSys *)nvfbcLibrary.create(NVFBC_TO_SYS, &maxDisplayWidth, &maxDisplayHeight);
					if (!nvfbcToSys)
					{
						fprintf(stderr, "Unable to create an instance of NvFBC\n");
						goto quit;
					}
					//! Setup the frame grab
					NVFBC_TOSYS_SETUP_PARAMS fbcSysSetupParams = { 0 };
					fbcSysSetupParams.dwVersion = NVFBC_TOSYS_SETUP_PARAMS_VER;
					fbcSysSetupParams.eMode = args.eBufFormat;
					fbcSysSetupParams.bWithHWCursor = args.bHWCursor;
					fbcSysSetupParams.bDiffMap = FALSE;
					fbcSysSetupParams.ppBuffer = (void **)&frameBuffer;
					fbcSysSetupParams.ppDiffMap = NULL;
					status = nvfbcToSys->NvFBCToSysSetUp(&fbcSysSetupParams);
					cnt--;
					if (status == NVFBC_SUCCESS)
					{
						bRecoveryDone = TRUE;
					}
					else
					{
						fprintf(stderr, "Unable to recover from NvFBC Frame grab failure.\n");
						goto quit;
					}
				}
			}
		}
	}
	if (status != NVFBC_SUCCESS)
	{
		fprintf(stderr, "Unable to setup frame grab.\n");
	}*/

	m_OutputNumber = output;
	m_Device = device;

	// Get DXGI device
	IDXGIDevice* DxgiDevice = nullptr;
	HRESULT hr = m_Device->m_dxDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&DxgiDevice));
	if (FAILED(hr))
	{
		return false;
	}

	// Get DXGI adapter
	IDXGIAdapter* DxgiAdapter = nullptr;
	hr = DxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&DxgiAdapter));
	DxgiDevice->Release();
	DxgiDevice = nullptr;
	if (FAILED(hr))
	{
		return false;
	}

	// Get output
	IDXGIOutput* DxgiOutput = nullptr;
	hr = DxgiAdapter->EnumOutputs(output, &DxgiOutput);
	DxgiAdapter->Release();
	DxgiAdapter = nullptr;
	if (FAILED(hr))
	{
		return false;
	}

	DxgiOutput->GetDesc(&m_OutputDesc);

	// QI for Output 1
	IDXGIOutput1* DxgiOutput1 = nullptr;
	hr = DxgiOutput->QueryInterface(__uuidof(DxgiOutput1), reinterpret_cast<void**>(&DxgiOutput1));
	DxgiOutput->Release();
	DxgiOutput = nullptr;
	if (FAILED(hr))
	{
		return false;
	}

	// Create desktop duplication
	hr = DxgiOutput1->DuplicateOutput(m_Device->m_dxDevice, &m_DeskDupl);
	DxgiOutput1->Release();
	DxgiOutput1 = nullptr;
	if (FAILED(hr))
	{
		if (hr == DXGI_ERROR_NOT_CURRENTLY_AVAILABLE)
		{
			MessageBoxW(nullptr, L"There is already the maximum number of applications using the Desktop Duplication API running, please close one of those applications and then try again.", L"Error", MB_OK);
			return false;
		}
		else if (hr == DXGI_ERROR_UNSUPPORTED)
		{
			MessageBoxW(nullptr, L"Not supported", L"Error", MB_OK);
			return false;
		}
		return false;
	}

	return true;

}

void ScreenCapturer::EndFrame()
{
	if (!m_DeskDupl)
		return;
	HRESULT hr = m_DeskDupl->ReleaseFrame();
	if (FAILED(hr))
	{
		return;
	}

	if (m_AcquiredDesktopImage)
	{
		m_AcquiredDesktopImage->Release();
		m_AcquiredDesktopImage = nullptr;
	}
	return;
}

Texture2D* ScreenCapturer::GetFrame()
{
	IDXGIResource* DesktopResource = nullptr;
	DXGI_OUTDUPL_FRAME_INFO FrameInfo;

	// Get new frame
	HRESULT hr = m_DeskDupl->AcquireNextFrame(500, &FrameInfo, &DesktopResource);
	if (hr == DXGI_ERROR_WAIT_TIMEOUT)
	{
		return nullptr;
	}

	if (FAILED(hr))
	{
		return nullptr;
	}

	// If still holding old frame, destroy it
	if (m_AcquiredDesktopImage)
	{
		m_AcquiredDesktopImage->Release();
		m_AcquiredDesktopImage = nullptr;
	}

	// QI for IDXGIResource
	hr = DesktopResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&m_AcquiredDesktopImage));
	DesktopResource->Release();
	DesktopResource = nullptr;
	if (FAILED(hr))
	{
		return nullptr;
	}

	// Get metadata
	if (FrameInfo.TotalMetadataBufferSize)
	{
		// Old buffer too small
		if (FrameInfo.TotalMetadataBufferSize > m_MetaDataSize)
		{
			if (m_MetaDataBuffer)
			{
				delete[] m_MetaDataBuffer;
				m_MetaDataBuffer = nullptr;
			}
			m_MetaDataBuffer = new byte_t[FrameInfo.TotalMetadataBufferSize];
			if (!m_MetaDataBuffer)
			{
				m_MetaDataSize = 0;
				//Data->MoveCount = 0;
				//Data->DirtyCount = 0;
				return nullptr;
			}
			m_MetaDataSize = FrameInfo.TotalMetadataBufferSize;
		}

		UINT BufSize = FrameInfo.TotalMetadataBufferSize;

		// Get move rectangles
		hr = m_DeskDupl->GetFrameMoveRects(BufSize, reinterpret_cast<DXGI_OUTDUPL_MOVE_RECT*>(m_MetaDataBuffer), &BufSize);
		if (FAILED(hr))
		{
			//Data->MoveCount = 0;
			//Data->DirtyCount = 0;
			return nullptr;
		}
		//Data->MoveCount = BufSize / sizeof(DXGI_OUTDUPL_MOVE_RECT);

		BYTE* DirtyRects = m_MetaDataBuffer + BufSize;
		BufSize = FrameInfo.TotalMetadataBufferSize - BufSize;

		// Get dirty rectangles
		hr = m_DeskDupl->GetFrameDirtyRects(BufSize, reinterpret_cast<RECT*>(DirtyRects), &BufSize);
		if (FAILED(hr))
		{
			//Data->MoveCount = 0;
			//Data->DirtyCount = 0;
			return nullptr;
		}
		//Data->DirtyCount = BufSize / sizeof(RECT);

		//Data->MetaData = m_MetaDataBuffer;
	}
	Texture2D* texture = nullptr;
	ID3D11Texture2D * textureResource = m_AcquiredDesktopImage;
	texture = new Texture2D(m_Device, textureResource);
	//Data->FrameInfo = FrameInfo;

	return texture;
}

Image* ScreenCapturer::GetImage()
{
	return new Image(m_MetaDataBuffer, 3, Window::GetDesktopResolution());
}
