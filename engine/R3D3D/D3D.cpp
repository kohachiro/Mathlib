#include "R3D3D.h"
R3D3D *g_R3D3D=NULL;

bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, 
                                  D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext );
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext );
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, 
                                const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext );
void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, 
                          bool* pbNoFurtherProcessing, void* pUserContext );
void CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext );
void CALLBACK OnLostDevice( void* pUserContext );
void CALLBACK OnDestroyDevice( void* pUserContext );

HRESULT CreateRenderDevice(HINSTANCE hDLL, R3RenderDevice **pDevice)
{
	if(!*pDevice) 
	{
		*pDevice = new R3D3D(hDLL);
		return S_OK;
	}
	return E_FAIL;
}

HRESULT ReleaseRenderDevice(R3RenderDevice **pDevice)
{
	if(!*pDevice) 
		return E_FAIL;
	delete *pDevice;
	*pDevice = NULL;
	return S_OK;
}

R3D3D::R3D3D(HINSTANCE hDLL)
{
	m_hDLL				= hDLL;
	m_pDC				= NULL;
	m_pDevice			= NULL;
	m_pD3DDialogManager	= new R3D3DDialogManager(m_pDevice);
	g_R3D3D				= this;
}

R3D3D::~R3D3D()
{
	this->Release();
}
void R3D3D::Release()
{
	if(m_pDevice)
	{
		m_pDevice->Release();
		m_pDevice = NULL;
	}
	if(m_pDC)
	{
		m_pDC->Release();
		m_pDC = NULL;
	}
	delete m_pD3DDialogManager;
}
HRESULT R3D3D::Init( bool isWindows, int width, int height )
{
    DXUTSetCallbackD3D9DeviceAcceptable( IsDeviceAcceptable );
    DXUTSetCallbackD3D9DeviceCreated( OnCreateDevice );
    DXUTSetCallbackD3D9FrameRender( OnFrameRender );
    DXUTSetCallbackD3D9DeviceReset( OnResetDevice );
    DXUTSetCallbackD3D9DeviceLost( OnLostDevice );
    DXUTSetCallbackD3D9DeviceDestroyed( OnDestroyDevice );
    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackFrameMove( OnFrameMove );
    DXUTSetCallbackKeyboard( KeyboardProc );
    DXUTSetCallbackDeviceChanging( ModifyDeviceSettings );
	
    DXUTInit( true, true ); // Parse the command line, handle the default hotkeys, and show msgboxes
    DXUTSetCursorSettings( true, true ); // Show the cursor and clip it when in full screen
    DXUTCreateWindow( L"EmptyProject" );
    DXUTCreateDevice(  isWindows, width, height );
    // Start the render loop
    DXUTMainLoop();
    return DXUTGetExitCode();
}
