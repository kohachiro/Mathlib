#include "R3Renderer.h"

R3Renderer::R3Renderer(HINSTANCE hInst)
{
	m_hDLL		= NULL;
	m_pDevice	= NULL;
}

R3Renderer::~R3Renderer(void)
{
}

HRESULT R3Renderer::CreateDevice(int iAPI)
{
	if (iAPI == DIRECT3D)
	{
		m_hDLL = LoadLibrary(L"R3D3D.dll");
		if(!m_hDLL)
		{
			MessageBox(NULL,L"Loading R3D3D.dll from lib failed.",L"R3Engine - error", MB_OK | MB_ICONERROR);
			return E_FAIL;
		}
	}
	else if (iAPI == OPENGL)
	{
		m_hDLL = LoadLibrary(L"R3GL.dll");
		if(!m_hDLL)
		{
			MessageBox(NULL,L"Loading R3GL.dll from lib failed.",L"R3Engine - error", MB_OK | MB_ICONERROR);
			return E_FAIL;
		}
	}
	else 
	{
		MessageBox(NULL, L"API not yet supported.", L"R3Engine - error", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}	

	// function pointer to dll's 'CreateRenderDevice' function
	CREATERENDERDEVICE _CreateRenderDevice = (CREATERENDERDEVICE)GetProcAddress(m_hDLL,"CreateRenderDevice");
	if ( !_CreateRenderDevice )
		return E_FAIL;
	// call dll's create function
	HRESULT hr = _CreateRenderDevice(m_hDLL, &m_pDevice);
	if(FAILED(hr))
	{
		MessageBox(NULL,L"CreateRenderDevice() from lib failed.",L"R3Engine - error", MB_OK | MB_ICONERROR);
		m_pDevice = NULL;
		return E_FAIL;
	}
	return S_OK;
}
void R3Renderer::Release(void) {
	// function pointer to dll 'ReleaseRenderDevice' function
	RELEASERENDERDEVICE _ReleaseRenderDevice = (RELEASERENDERDEVICE)GetProcAddress(m_hDLL, "ReleaseRenderDevice");
	// call dll's release function
	if (m_pDevice) {
		if(FAILED(_ReleaseRenderDevice(&m_pDevice))){
			m_pDevice = NULL;
		}
	}
} // Release
