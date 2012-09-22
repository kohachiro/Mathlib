#include "R3D3D.h"

/**//***
程序启动：InitApp →MsgProc →IsDeviceAcceptable →ModifyDeviceSettings → OnCreateDevice →OnResetDevice → 渲染主循环 
渲染主循环：OnFrameMove → OnFrameRender 
改变设备：ModifyDeviceSettings → OnLostDevice →根据需要调用OnDestroyDevice → OnResetDevice → 渲染主循环 
程序退出：OnLostDevice →OnDestroyDevice
***/
bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, 
                                  D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
    // Skip backbuffer formats that don't support alpha blending
    IDirect3D9* pD3D = DXUTGetD3D9Object();
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                    AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, 
                    D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
        return false;

    return true;
}


//--------------------------------------------------------------------------------------
// Before a device is created, modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext )
{
    return true;
}


//--------------------------------------------------------------------------------------
// Create any D3DPOOL_MANAGED resources here 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    HRESULT hr;
	for (std::vector<R3Font *>::iterator i=g_R3D3D->m_pD3DDialogManager->m_vR3Font.begin();i!=g_R3D3D->m_pD3DDialogManager->m_vR3Font.end();i++)
		V_RETURN( D3DXCreateFont( pd3dDevice, (*i)->Height, (*i)->Width, (*i)->Weight, (*i)->MipLevels, (*i)->Italic, (*i)->CharSet, 
			(*i)->OutputPrecision, (*i)->Quality, (*i)->PitchAndFamily, 
			(*i)->pFacename, &(*i)->pFont ) );

	V_RETURN( g_R3D3D->m_pD3DDialogManager->m_DialogResourceManager.OnD3D9CreateDevice( pd3dDevice ) );
	V_RETURN( g_R3D3D->m_pD3DDialogManager->m_SettingsDlg.OnD3D9CreateDevice( pd3dDevice ) );
	return S_OK;
}


//--------------------------------------------------------------------------------------
// Create any D3DPOOL_DEFAULT resources here 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, 
                                const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	HRESULT hr;
	for (std::vector<R3Font *>::iterator i=g_R3D3D->m_pD3DDialogManager->m_vR3Font.begin();i!=g_R3D3D->m_pD3DDialogManager->m_vR3Font.end();i++)
		if ( (*i)->pFont ) V_RETURN( (*i)->pFont->OnResetDevice() );
		//if( g_R3D3D->m_pD3DDialogManager->m_pFont ) V_RETURN( g_R3D3D->m_pD3DDialogManager->m_pFont->OnResetDevice() );
	V_RETURN( D3DXCreateSprite( pd3dDevice, &g_R3D3D->m_pD3DDialogManager->m_pSprite ) );
	//for (std::vector<R3Font *>::iterator i=g_R3D3D->m_pD3DDialogManager->m_vR3Font.begin();i!=g_R3D3D->m_pD3DDialogManager->m_vR3Font.end();i++){
	//		CDXUTTextHelper* pText = new CDXUTTextHelper( *i,g_R3D3D->m_pD3DDialogManager->m_pSprite,NULL, NULL,15);
	//		(*i)->Text.push_back( pText );
	//}

	V_RETURN( g_R3D3D->m_pD3DDialogManager->m_DialogResourceManager.OnD3D9ResetDevice() );
	V_RETURN( g_R3D3D->m_pD3DDialogManager->m_SettingsDlg.OnD3D9ResetDevice( ) );
	//std::vector<R3Dialog *> m= ((R3D3DDialogManager*)g_R3D3D->GetR3DialogManager())->m_vR3Dialog;
	for (std::vector<R3Dialog *>::iterator i=g_R3D3D->m_pD3DDialogManager->m_vR3Dialog.begin();i!=g_R3D3D->m_pD3DDialogManager->m_vR3Dialog.end();i++){
	//for (int i=0;i != m.size();i++){
		(*i)->dialog->SetLocation( (*i)->x, (*i)->y );
		(*i)->dialog->SetSize( (*i)->width,(*i)->height );
		(*i)->dialog->SetBackgroundColors( (*i)->colorTopLeft,(*i)->colorTopRight,(*i)->colorBottomLeft, (*i)->colorBottomRight );
	}

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
}


//--------------------------------------------------------------------------------------
// Render the scene 
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
    HRESULT hr;

    // Clear the render target and the zbuffer 
    V( pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 45, 50, 170), 1.0f, 0) );
	if (g_R3D3D->m_pD3DDialogManager->m_SettingsDlg.IsActive()){
		g_R3D3D->m_pD3DDialogManager->m_SettingsDlg.OnRender( fElapsedTime ) ;
		return;
	}
    // Render the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
		for (std::vector<R3Font *>::iterator i=g_R3D3D->m_pD3DDialogManager->m_vR3Font.begin();i!=g_R3D3D->m_pD3DDialogManager->m_vR3Font.end();i++)
		{
			CDXUTTextHelper txtHelper( (*i)->pFont, g_R3D3D->m_pD3DDialogManager->m_pSprite, 15 );
			txtHelper.Begin();
			for (std::vector<R3Text *>::iterator j=(*i)->Text.begin();j!=(*i)->Text.end();j++)
			{
				txtHelper.SetInsertionPos( (*j)->x, (*j)->y );
				txtHelper.SetForegroundColor( (*j)->color );
				txtHelper.DrawFormattedTextLine( (*j)->text );
			}
			txtHelper.End();
		}
		for (std::vector<R3Dialog *>::iterator i=g_R3D3D->m_pD3DDialogManager->m_vR3Dialog.begin();i!=g_R3D3D->m_pD3DDialogManager->m_vR3Dialog.end();i++)
			(*i)->dialog->OnRender( fElapsedTime ) ;
        V( pd3dDevice->EndScene() );
    }
}
//--------------------------------------------------------------------------------------
// Handle messages to the application 
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, 
                          bool* pbNoFurtherProcessing, void* pUserContext )
{

	if( g_R3D3D->m_pD3DDialogManager->m_SettingsDlg.IsActive() )
    {
        g_R3D3D->m_pD3DDialogManager->m_SettingsDlg.MsgProc( hWnd, uMsg, wParam, lParam );
        return 0;
    }
	for (std::vector<R3Dialog *>::iterator i=g_R3D3D->m_pD3DDialogManager->m_vR3Dialog.begin();i!=g_R3D3D->m_pD3DDialogManager->m_vR3Dialog.end();i++)
	{
		*pbNoFurtherProcessing = (*i)->dialog->MsgProc( hWnd, uMsg, wParam, lParam );
	    if( *pbNoFurtherProcessing )
		    return 0;
	}
    return 0;
}

//--------------------------------------------------------------------------------------
// proc key to the application 
//--------------------------------------------------------------------------------------
void CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
}
//--------------------------------------------------------------------------------------
// Release resources created in the OnResetDevice callback here 
//--------------------------------------------------------------------------------------
void CALLBACK OnLostDevice( void* pUserContext )
{

	SAFE_RELEASE( g_R3D3D->m_pD3DDialogManager->m_pSprite );

	for (std::vector<R3Font *>::iterator i=g_R3D3D->m_pD3DDialogManager->m_vR3Font.begin();i!=g_R3D3D->m_pD3DDialogManager->m_vR3Font.end();i++)
		if( (*i)->pFont ) (*i)->pFont->OnLostDevice();
	g_R3D3D->m_pD3DDialogManager->m_DialogResourceManager.OnD3D9LostDevice();
	g_R3D3D->m_pD3DDialogManager->m_SettingsDlg.OnD3D9LostDevice( );
}


//--------------------------------------------------------------------------------------
// Release resources created in the OnCreateDevice callback here
//--------------------------------------------------------------------------------------
void CALLBACK OnDestroyDevice( void* pUserContext )
{
	for (std::vector<R3Font *>::iterator i=g_R3D3D->m_pD3DDialogManager->m_vR3Font.begin();i!=g_R3D3D->m_pD3DDialogManager->m_vR3Font.end();i++)
		SAFE_RELEASE( (*i)->pFont );
		
	g_R3D3D->m_pD3DDialogManager->m_DialogResourceManager.OnD3D9DestroyDevice();
	g_R3D3D->m_pD3DDialogManager->m_SettingsDlg.OnD3D9DestroyDevice( );
}
